#ifndef _AUDIO_MANAGER_H_


#pragma once
#include "Entity.h"
#include <unordered_map>
#include "TransformManager.h"
#include <Audio.h>
#include "Flags.h"

class AudioMananger
{
public:
	CCR(AudioType, uint32_t, NUM_TYPES = 5,
		BG = 1 << 0,
		Effect = 1 << 1,
		Looping = 1 << 2,
		Transition = 1 << 3,
		Single = 1 << 4);
	struct AudioData
	{
		AudioData(uint32_t GUID) :GUID(GUID), type(0), position(DirectX::XMFLOAT3()), fileInfo(Audio::FileInfo()), progress(0U)
		{

		}
		uint32_t GUID;
		DirectX::XMFLOAT3 position;
		Audio::FileInfo fileInfo;
		uint32_t progress;
		AudioType type;
	};

	AudioMananger(TransformManager& transformManager);
	~AudioMananger();


	const void BindEntity(const Entity& entity);
	const void AddAudio(const Entity& entity, char* path, const AudioType& type);
	const void StartAudio(const Entity& entity);
private:
	void _TransformChanged(const Entity& entity, const DirectX::XMMATRIX& transform, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& up);

	
	std::unordered_map<Entity, AudioData*, EntityHasher> _entityToData;
};


#endif // !_AUDIO_MANAGER_H_
