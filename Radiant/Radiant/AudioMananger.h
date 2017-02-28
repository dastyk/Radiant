#ifndef _AUDIO_MANAGER_H_


#pragma once
#include "Entity.h"
#include <unordered_map>
#include "TransformManager.h"
#include "CameraManager.h"
#include <Audio.h>
#include "Flags.h"
CCR(AudioType, uint32_t, NUM_TYPES = 6,
	BG = 1 << 0,
	Effect = 1 << 1,
	Looping = 1 << 2,
	Transition = 1 << 3,
	Single = 1 << 4,
	Positioned = 1 << 5);
class AudioMananger
{
public:

	struct AudioData
	{
		AudioData(uint32_t GUID, DirectX::XMFLOAT3* listenerPos) :GUID(GUID), type(0), fileInfo(Audio::FileInfo()), progress(0U) , audioPos(DirectX::XMFLOAT3()), listenerPos(listenerPos)
		{

		}
		uint32_t GUID;
		Audio::FileInfo fileInfo;
		uint32_t progress;
		AudioType type;
		DirectX::XMFLOAT3 audioPos;
		DirectX::XMFLOAT3* listenerPos;
	};

	AudioMananger(TransformManager& transformManager, CameraManager& cameraManager);
	~AudioMananger();


	const void BindEntity(const Entity& entity);
	const void AddAudio(const Entity& entity, char* path, const AudioType& type);
	const void StartAudio(const Entity& entity);
private:
	void _TransformChanged(const Entity& entity, const DirectX::XMMATRIX& transform, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& up);
	void _CameraChanged(const DirectX::XMFLOAT3& pos);
	
	std::unordered_map<Entity, AudioData*, EntityHasher> _entityToData;

	DirectX::XMFLOAT3 _positionedListernerPos;
};


#endif // !_AUDIO_MANAGER_H_
