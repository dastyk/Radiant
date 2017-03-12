#include "AudioMananger.h"
using namespace DirectX;
//#include "DspFilters/Dsp.h"

static std::vector<float> bandPassCoeff;
void convolve(const float Signal[/* SignalLen */], size_t SignalLen, size_t channels,
	const float Kernel[/* KernelLen */], size_t KernelLen,
	float Result[/* SignalLen + KernelLen - 1 */])
{
	size_t n;

	for (n = 0; n < SignalLen + KernelLen - 1; n++)
	{
		size_t kmin, kmax, k;

		Result[n] = 0;

		kmin = (n >= KernelLen - 1) ? n - (KernelLen - 1) : 0;
		kmax = (n < SignalLen - 1) ? n : SignalLen - 1;

		for (k = kmin; k <= kmax; k++)
		{
			Result[n] += Signal[k*channels] * Kernel[n - k];
		}
	}
}


CallbackPrototype(BGCallback)
{
	auto& info = *((AudioMananger::AudioData*)userData);
	float* out = (float*)outputBuffer;
	size_t offset = info.progress *framesPerBuffer;
	PaStreamCallbackResult result = paContinue;
	if (offset >= fileInfo.info.frames)
		return paComplete;

	for (unsigned long i = 0; i < framesPerBuffer; i++)
	{

		if (offset + i < fileInfo.info.frames)
		{
			*out++ = fileInfo.data[offset*info.fileInfo.info.channels + i*info.fileInfo.info.channels] * info.volume;
			*out++ = fileInfo.data[offset*info.fileInfo.info.channels + i*info.fileInfo.info.channels + 1 % info.fileInfo.info.channels] * info.volume;

		}
		else
		{
			*out++ = 0.0f;
			*out++ = 0.0f;
		}
	}

	
	info.progress++;
	return result;
}
CallbackPrototype(EffectCallback)
{
	auto& info = *((AudioMananger::AudioData*)userData);
	float* out = (float*)outputBuffer;
	size_t offset = info.progress*framesPerBuffer;
	if (offset >= fileInfo.info.frames)
		return paComplete;
	XMVECTOR aPos = XMLoadFloat3(&info.audioPos);
	if (info.type & AudioType::On_Move)
	{
		XMVECTOR aPosP = XMLoadFloat3(&info.audioPosPrev);
		XMVECTOR aTol = aPos - aPosP;
		float dist = XMVectorGetX(XMVectorAbs(aTol)); 
		
		if(info.counter % (2) == 0)
			info.audioPosPrev = info.audioPos;
		if (dist < 0.000001f)
		{
			memset(outputBuffer, 0, fileInfo.info.channels* framesPerBuffer * sizeof(float));
			
			return paContinue;
		}
		info.counter += 1;
	}
	auto vRight = info.volume;
	auto vLeft = info.volume;
	
	XMVECTOR lPos = XMLoadFloat3(info.listenerPos);
	XMVECTOR aTol = aPos - lPos;
	float dist = XMVectorGetX(XMVector3Length(aTol));
	if (info.type & AudioType::Radio)
	{
		if (dist > 8.0f)
		{
			// Add the bandpass filter
			size_t chunk = framesPerBuffer;
			if (offset + framesPerBuffer >= fileInfo.info.frames)
				chunk = (offset + framesPerBuffer - fileInfo.info.frames) / 4;

			float * c1c = new float[framesPerBuffer + bandPassCoeff.size() - 1];
			float * c2c = new float[framesPerBuffer + bandPassCoeff.size() - 1];
			memset(c1c, 0, sizeof(float)*(framesPerBuffer + bandPassCoeff.size() - 1));
			memset(c2c, 0, sizeof(float)*(framesPerBuffer + bandPassCoeff.size() - 1));
			convolve(&fileInfo.data[offset*info.fileInfo.info.channels], chunk, info.fileInfo.info.channels, bandPassCoeff.data(), bandPassCoeff.size(), c1c);
			convolve(&fileInfo.data[offset*info.fileInfo.info.channels + 1 % info.fileInfo.info.channels], chunk, info.fileInfo.info.channels, bandPassCoeff.data(), bandPassCoeff.size(), c2c);

			for (unsigned long i = 0; i < framesPerBuffer; i++)
			{

					*out++ = c1c[i];
					*out++ = c2c[i];
	
			}
			info.progress++;

			delete[] c1c;
			delete[] c2c;
			return paContinue;
		}
	}
	if (info.type & AudioType::Sterio_Pan)
	{	
		// Apply Sterio Pan
		aTol = XMVector3Normalize(aTol);
		auto right = XMLoadFloat3(info.listenerRight);
		auto forward = XMLoadFloat3(info.listenerDir);
		float aF = XMVectorGetX(XMVector3Dot(aTol, forward));
		float aR = XMVectorGetX(XMVector3Dot(aTol, right));
		aF = exp2f(aF) / exp2f(1.0f);
		auto volume = 1.0f - exp2f(-(1 / dist));
		vRight *= fminf(fmaxf(aR, 0.0f) + fabsf(aF), 1.0f) * volume;
		vLeft *= fminf(fmaxf(-aR, 0.0f) + fabsf(aF), 1.0f)* volume;

		
	}


	for (unsigned long i = 0; i < framesPerBuffer; i++)
	{

		if (offset + i < fileInfo.info.frames)
		{
			*out++ = fileInfo.data[offset*info.fileInfo.info.channels + i*info.fileInfo.info.channels] * vLeft;
			*out++ = fileInfo.data[offset*info.fileInfo.info.channels + i*info.fileInfo.info.channels + 1 % info.fileInfo.info.channels] * vRight;
		}
		else
		{
			*out++ = 0.0f;
			*out++ = 0.0f;
		}
	}
	info.progress++;
	return paContinue;
}
FinishedCallbackPrototype(LoopFinishedCallback)
{
	auto& info = *((AudioMananger::AudioData*)userData);
	info.progress = 0;
	return paContinue;
}

FinishedCallbackPrototype(StopFinishedCallback)
{
	return paComplete;
}

#include <fstream>
AudioMananger::AudioMananger(TransformManager& transformManager, CameraManager& cameraManager)
{
	cameraManager.cameraChanged += Delegate<void(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& forward, const DirectX::XMVECTOR& right)>::Make<AudioMananger, &AudioMananger::_CameraChanged>(this);
	transformManager.TransformChanged += Delegate<void(const Entity&, const XMMATRIX&, const XMVECTOR&, const DirectX::XMVECTOR&, const DirectX::XMVECTOR&, const XMVECTOR&)>::Make<AudioMananger, &AudioMananger::_TransformChanged>(this);

	fstream in;
	in.open("Audio/bandpass.m", std::ios::in);

	if (!in.is_open())
		throw std::runtime_error("Failed");

	double value;
	while (in >> value)
	{
		bandPassCoeff.push_back(value);
	}

	in.close();

}


AudioMananger::~AudioMananger()
{
	auto a = Audio::GetInstance();
	for (auto& e : _entityToData)
	{
		a->StopStream(e.second->GUID);
		delete e.second;
	}
}

const void AudioMananger::BindEntity(const Entity & entity)
{
	auto& find = _entityToData.find(entity);
	if (find == _entityToData.end())
	{
		uint32_t GUID = _entityToData.size();
		_entityToData[entity] = new AudioData(GUID, &_positionedListernerPos, &_positionedListernerForward, &_positionedListernerRight);
	}
}

const void AudioMananger::AddAudio(const Entity & entity, char * path, const AudioType & type)
{
	auto& find = _entityToData.find(entity);
	if (find != _entityToData.end())
	{
		auto callback = BGCallback;
		if (type & AudioType::BG)
			callback = BGCallback;
		else if (type & AudioType::Effect)
			callback = EffectCallback;
			
		auto a = Audio::GetInstance();
		auto fI = a->ReadFile(path);
		find->second->fileInfo = fI;
		find->second->type = type;
		fI.info.channels = 2; 
		find->second->counter = 0;
		a->CreateOutputStream(callback, find->second, fI, 8192, find->second->GUID);
	}
}

const void AudioMananger::StartAudio(const Entity & entity, float vol)
{
	auto& find = _entityToData.find(entity);
	if (find != _entityToData.end())
	{
		auto& type = find->second->type;
		auto a = Audio::GetInstance();
		auto callback = StopFinishedCallback;
		if(type & AudioType::Single)
			callback = StopFinishedCallback;
		else if (type & AudioType::Looping)
			callback = LoopFinishedCallback;
		find->second->volume = vol;
		a->StartStream(find->second->GUID, callback, find->second);
	}
}

void AudioMananger::_TransformChanged(const Entity & entity, const DirectX::XMMATRIX & transform, const DirectX::XMVECTOR & pos, const DirectX::XMVECTOR & dir, const DirectX::XMVECTOR& right, const DirectX::XMVECTOR & up)
{
	auto& find = _entityToData.find(entity);
	if (find != _entityToData.end())
	{
		XMStoreFloat3(&find->second->audioPos, pos);
		XMStoreFloat3(&find->second->audioDir, dir);
	}
}

void AudioMananger::_CameraChanged(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& forward, const DirectX::XMVECTOR& right)
{
	XMStoreFloat3(&_positionedListernerPos, pos);
	XMStoreFloat3(&_positionedListernerForward, forward);
	XMStoreFloat3(&_positionedListernerRight, right);
}
