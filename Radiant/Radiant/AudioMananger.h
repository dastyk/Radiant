#ifndef _AUDIO_MANAGER_H_


#pragma once
#include "Entity.h"
#include <unordered_map>
#include "TransformManager.h"
#include <Audio.h>

class AudioMananger
{
public:
	struct AudioData
	{
		AudioData(uint32_t GUID) :GUID(GUID), position(DirectX::XMFLOAT3()), fileInfo(Audio::FileInfo())
		{

		}
		uint32_t GUID;
		DirectX::XMFLOAT3 position;
		Audio::FileInfo fileInfo;
	};

	AudioMananger(TransformManager& transformManager);
	~AudioMananger();


	const void BindEntity(const Entity& entity);
	const void AddBGAudio(const Entity& entity, char* path);
	const void AddPositionAudio(const Entity& entity, char* path);
	const void StartAudio(const Entity& entity);
private:
	void _TransformChanged(const Entity& entity, const DirectX::XMMATRIX& transform, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& up);

	
	std::unordered_map<Entity, AudioData*, EntityHasher> _entityToData;
};


#endif // !_AUDIO_MANAGER_H_
