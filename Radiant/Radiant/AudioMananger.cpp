#include "AudioMananger.h"
using namespace DirectX;


CallbackPrototype(BGCallback)
{
	auto& info = *((AudioMananger::AudioData*)userData);
	size_t offset = info.progress *framesPerBuffer;
	size_t chunk = framesPerBuffer*info.fileInfo.info.channels;
	PaStreamCallbackResult result = paContinue;
	float* out = (float*)outputBuffer;
	if (offset + chunk >= fileInfo.info.frames)
	{
		result = paComplete;
		chunk = fileInfo.info.frames - offset;
	}
	memcpy(outputBuffer, fileInfo.data + offset, chunk*sizeof(float));
	/*for (unsigned long i = 0; i < framesPerBuffer; i++)
	{
		if (offset + i < fileInfo.info.frames)
		{


			*out++ = fileInfo.data[offset + i*2] ;
			*out++ = fileInfo.data[offset + i*2 +1];
		}
		else
		{
			*out++ = 0.0f;
			*out++ = 0.0f;
		}
	}*/
	info.progress += 2;
	return result;
	//static uint32_t count = 0;
	//float* out = (float*)outputBuffer;
	//size_t offset = count*framesPerBuffer;
	//if (offset > fileInfo.info.frames)
	//{
	//	count = 0;
	//	return paComplete;
	//}
	//for (unsigned long i = 0; i < framesPerBuffer; i++)
	//{
	//	if (offset + i < fileInfo.info.frames)
	//	{


	//		*out++ = fileInfo.data[offset + i*2] * (1.0f - (offset / (float)fileInfo.info.frames));
	//		*out++ = fileInfo.data[offset + i*2 + 1] * (offset / (float)fileInfo.info.frames);
	//	}
	//	else
	//	{
	//		*out++ = 0.0f;
	//		*out++ = 0.0f;
	//	}
	//}
	////	memcpy(outputbuffer, (float*)data.info.data + offset, sizeof(float)*framesperbuffer);
	//count+=1;
	//return paContinue;
}
CallbackPrototype(EffectCallback)
{
	auto& info = *((AudioMananger::AudioData*)userData);
	size_t offset = info.progress *framesPerBuffer;
	memcpy(outputBuffer, fileInfo.data, info.fileInfo.info.frames*info.fileInfo.info.channels);
	info.progress += info.fileInfo.info.channels;
	return paContinue;
}
FinishedCallbackPrototype(LoopFinishedCallback)
{
	return paContinue;
}

FinishedCallbackPrototype(StopFinishedCallback)
{
	return paContinue;
}



AudioMananger::AudioMananger(TransformManager& transformManager)
{
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
		_entityToData[entity] = new AudioData(GUID);
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

		a->StartStream(find->second->GUID, callback, nullptr);
	}
}

void AudioMananger::_TransformChanged(const Entity & entity, const DirectX::XMMATRIX & transform, const DirectX::XMVECTOR & pos, const DirectX::XMVECTOR & dir, const DirectX::XMVECTOR & up)
{
}
