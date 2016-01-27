#ifndef _CAMERAMANAGER_H_
#define _CAMERAMANAGER_H_

#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
#include "Entity.h"
#include <map>

//#include "Material.h"
#include "TransformManager.h"
#include "ICameraProvider.h"
#include "Graphics.h"


class CameraManager : public ICameraProvider
{
public:
	CameraManager(Graphics& graphics, TransformManager& transformManager);
	~CameraManager();

	const void CreateCamera(Entity entity);

	const void SetActivePerspective(Entity& entity);

	//Material& GetMaterial( Entity entity, std::uint32_t part );
	//void SetMaterial( Entity entity, std::uint32_t part, const Material& material );

	//void GatherCam(std::function<void(CamData&)> ProvideCam);
	void GatherCam(CamData& Cam);


private:
	struct CameraData
	{
		Entity OwningEntity;



		float fov;
		float aspect;
		float nearp;
		float farp;

		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT4X4 projectionMatrix;
		DirectX::XMFLOAT4X4 viewProjectionMatrix;


		CameraData(Entity e)
		{
			OwningEntity = e;

			aspect = 1.0;
			fov = 90.0f;
			nearp = 1.0f;
			farp = 100.0f;
		}
	};

private:
	const void TransformChanged(Entity entity, const DirectX::XMVECTOR & pos, const DirectX::XMVECTOR & lookAt, const DirectX::XMVECTOR & up);
private:
	std::vector<CameraData> _cameras;
	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
	Entity _activePerspective;
};
#endif