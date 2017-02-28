#include "AudioMananger.h"
using namespace DirectX;


CallbackPrototype(BGCallback)
{
	auto& info = *((AudioMananger::AudioData*)userData);
	size_t offset = info.progress *framesPerBuffer;
	size_t chunk = framesPerBuffer*info.fileInfo.info.channels;
	PaStreamCallbackResult result = paContinue;
	if (offset >= fileInfo.info.frames)
		return paComplete;
	if (offset + chunk >= fileInfo.info.frames)
	{
		//result = paComplete;
		chunk = fileInfo.info.frames - offset;
		memcpy(outputBuffer, fileInfo.data + offset, chunk * sizeof(float));
		memset((float*)outputBuffer + chunk, 0, (framesPerBuffer*fileInfo.info.channels - chunk) * sizeof(float));
	}
	else
		memcpy(outputBuffer, fileInfo.data + offset, chunk*sizeof(float));
	info.progress += 2;
	return result;
}
CallbackPrototype(EffectCallback)
{
	auto& info = *((AudioMananger::AudioData*)userData);
	float* out = (float*)outputBuffer;
	size_t offset = info.progress*framesPerBuffer;
	if (offset > fileInfo.info.frames)
	{
		info.progress = 0;
		return paComplete;
	}
	if (info.type & AudioType::Positioned)
	{
		// Apply position filter.
		for (unsigned long i = 0; i < framesPerBuffer; i++)
		{
			if (offset + i < fileInfo.info.frames)
			{


				*out++ = fileInfo.data[offset + i] * (1.0f - (offset / (float)fileInfo.info.frames));
				*out++ = fileInfo.data[offset + i] * (offset / (float)fileInfo.info.frames);
			}
			else
			{
				*out++ = 0.0f;
				*out++ = 0.0f;
			}
		}
	}
	info.progress +=1;
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
	return paContinue;
}



AudioMananger::AudioMananger(TransformManager& transformManager, CameraManager& cameraManager)
{
	cameraManager.cameraChanged += Delegate<void(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir)>::Make<AudioMananger, &AudioMananger::_CameraChanged>(this);
	transformManager.TransformChanged += Delegate<void(const Entity&, const XMMATRIX&, const XMVECTOR&, const XMVECTOR&, const XMVECTOR&)>::Make<AudioMananger, &AudioMananger::_TransformChanged>(this);
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
		_entityToData[entity] = new AudioData(GUID, &_positionedListernerPos, &_positionedListernerDir);
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

const void AudioMananger::StartAudio(const Entity & entity)
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

		a->StartStream(find->second->GUID, callback, find->second);
	}
}

void AudioMananger::_TransformChanged(const Entity & entity, const DirectX::XMMATRIX & transform, const DirectX::XMVECTOR & pos, const DirectX::XMVECTOR & dir, const DirectX::XMVECTOR & up)
{
	auto& find = _entityToData.find(entity);
	if (find != _entityToData.end())
	{
		XMStoreFloat3(&find->second->audioPos, pos);
		XMStoreFloat3(&find->second->audioDir, dir);
	}
}

void AudioMananger::_CameraChanged(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir)
{
	XMStoreFloat3(&_positionedListernerPos, pos);
	XMStoreFloat3(&_positionedListernerDir, dir);
}
