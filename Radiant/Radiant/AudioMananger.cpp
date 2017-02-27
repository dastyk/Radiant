#include "AudioMananger.h"
using namespace DirectX;


CallbackPrototype(BGMusicCallback)
{
	auto& info = *((AudioMananger::AudioData*)userData);




	return paContinue;
}

FinishedCallbackPrototype(LoopFinishedCallback)
{
	return paContinue;
}




AudioMananger::AudioMananger(TransformManager& transformManager)
{
	transformManager.TransformChanged += Delegate<void(const Entity&, const XMMATRIX&, const XMVECTOR&, const XMVECTOR&, const XMVECTOR&)>::Make<AudioMananger, &AudioMananger::_TransformChanged>(this);
}


AudioMananger::~AudioMananger()
{
	/*for (auto& e : _entityToData)
	{
		delete e.second;
	}*/
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

const void AudioMananger::AddBGAudio(const Entity & entity, char * path)
{
	auto& find = _entityToData.find(entity);
	if (find != _entityToData.end())
	{
		auto a = Audio::GetInstance();
		auto fI = a->ReadFile(path);
		find->second->fileInfo = fI;
		fI.info.channels = 2;
		a->CreateOutputStream(nullptr, find->second, fI, 256, find->second->GUID);
	}
	return void();
}

const void AudioMananger::AddPositionAudio(const Entity & entity, char * path)
{
	return void();
}

const void AudioMananger::StartAudio(const Entity & entity)
{
	auto& find = _entityToData.find(entity);
	if (find != _entityToData.end())
	{
		auto a = Audio::GetInstance();
		a->StartStream(find->second->GUID, LoopFinishedCallback, nullptr);
	}
}

void AudioMananger::_TransformChanged(const Entity & entity, const DirectX::XMMATRIX & transform, const DirectX::XMVECTOR & pos, const DirectX::XMVECTOR & dir, const DirectX::XMVECTOR & up)
{
}
