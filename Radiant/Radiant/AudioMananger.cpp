#include "AudioMananger.h"
using namespace DirectX;


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
			*out++ = fileInfo.data[offset + i*info.fileInfo.info.channels] * info.volume;
			*out++ = fileInfo.data[offset + i*info.fileInfo.info.channels + 1 % info.fileInfo.info.channels] * info.volume;
		}
		else
		{
			*out++ = 0.0f;
			*out++ = 0.0f;
		}
	}


	info.progress += info.fileInfo.info.channels;
	return result;
}
CallbackPrototype(EffectCallback)
{
	auto& info = *((AudioMananger::AudioData*)userData);
	float* out = (float*)outputBuffer;
	size_t offset = info.progress*framesPerBuffer;
	if (offset >= fileInfo.info.frames)
		return paComplete;
	if (info.type & AudioType::On_Move)
	{
		XMVECTOR aPos = XMLoadFloat3(&info.audioPos);
		XMVECTOR aPosP = XMLoadFloat3(&info.audioPosPrev);
		XMVECTOR aTol = aPos - aPosP;
		float dist = XMVectorGetX(XMVectorAbs(aTol)); 
		
		if(info.progress % (100*info.fileInfo.info.channels) == 0)
			info.audioPosPrev = info.audioPos;
		if (dist < 0.000001f)
		{
			memset(outputBuffer, 0, fileInfo.info.channels* framesPerBuffer * sizeof(float));
			info.progress += info.fileInfo.info.channels;
			return paContinue;
		}
	}
	auto vRight = info.volume;
	auto vLeft = info.volume;

	if (info.type & AudioType::Positioned)
	{	
		// Apply position filter.
		XMVECTOR aPos = XMLoadFloat3(&info.audioPos);
		XMVECTOR lPos = XMLoadFloat3(info.listenerPos);
		XMVECTOR aTol = aPos - lPos;
		float dist = XMVectorGetX(XMVectorAbs(aTol));
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
			*out++ = fileInfo.data[offset + i*info.fileInfo.info.channels] * vLeft;
			*out++ = fileInfo.data[offset + i*info.fileInfo.info.channels + 1 % info.fileInfo.info.channels] * vRight;
		}
		else
		{
			*out++ = 0.0f;
			*out++ = 0.0f;
		}
	}
	info.progress += info.fileInfo.info.channels;
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


AudioMananger::AudioMananger(TransformManager& transformManager, CameraManager& cameraManager)
{
	cameraManager.cameraChanged += Delegate<void(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& forward, const DirectX::XMVECTOR& right)>::Make<AudioMananger, &AudioMananger::_CameraChanged>(this);
	transformManager.TransformChanged += Delegate<void(const Entity&, const XMMATRIX&, const XMVECTOR&, const DirectX::XMVECTOR&, const DirectX::XMVECTOR&, const XMVECTOR&)>::Make<AudioMananger, &AudioMananger::_TransformChanged>(this);
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
		a->CreateOutputStream(callback, find->second, fI, 256, find->second->GUID);
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
