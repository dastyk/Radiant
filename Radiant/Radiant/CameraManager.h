#ifndef _CAMERAMANAGER_H_
#define _CAMERAMANAGER_H_

#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
#include "Entity.h"
#include <map>
#include "ICameraProvider.h"
//#include "Material.h"
//#include "TransformManager.h"




class CameraManager : public ICameraProvider
{
public:
	CameraManager(/*, TransformManager& transformManager*/);
	~CameraManager();

	const void CreateCamera(Entity entity, const char *filename, bool usePerspective);

	//Material& GetMaterial( Entity entity, std::uint32_t part );
	//void SetMaterial( Entity entity, std::uint32_t part, const Material& material );

private:
	struct CameraData
	{
		Entity OwningEntity;

		DirectX::XMVECTOR lookAt;
		DirectX::XMVECTOR up;
		DirectX::XMVECTOR lookDir;
		DirectX::XMVECTOR right;

		float fov;
		float aspect;
		float nearp;
		float farp;
		bool usePerspective;
		
		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT4X4 projectionMatrix;
		DirectX::XMFLOAT4X4 viewProjectionMatrix;

		CameraData(Entity e)
		{

			OwningEntity = e;
			lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			lookDir = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			right = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

			aspect = 1.0;
			fov = 90.0f;
			nearp = 1.0f;
			farp = 100.0f;
			usePerspective = false;
		}
	};

private:
	const void GatherCameras(std::function<void(CameraMatrixes&)> ProvideCamera);



	const void TransformChanged(Entity entity, const DirectX::XMFLOAT3& pos);

	DirectX::XMFLOAT4X4 _GetViewMatrix(Entity entity);
	DirectX::XMFLOAT4X4 _GetProjectionMatrix(Entity entity);
	DirectX::XMFLOAT4X4 _GetViewProjectionMatrix(Entity entity);
private:
	std::vector<CameraData> _cameras;
	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
};
#endif