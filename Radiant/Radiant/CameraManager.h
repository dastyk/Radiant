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
using namespace DirectX;

class CameraManager : public ICameraProvider
{
public:
	CameraManager( TransformManager& transformManager);
	~CameraManager();

	const void CreateCamera(Entity entity);

	const void SetActivePerspective(Entity& entity);

	void GatherCam(CamData& Cam);

	const void BindToRenderer(bool exclusive);

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


		CameraData(Entity& e) : OwningEntity(e), fov(90.0f), aspect(1.0f), nearp(0.1f), farp(100.0f), viewMatrix(DirectX::XMFLOAT4X4()), projectionMatrix(DirectX::XMFLOAT4X4()), viewProjectionMatrix(DirectX::XMFLOAT4X4())
		{
		}
	};

private:
	const void TransformChanged(Entity entity, const DirectX::XMVECTOR & pos, const DirectX::XMVECTOR & dir, const DirectX::XMVECTOR & up);
private:

	Graphics& _graphics;
	std::vector<CameraData> _cameras;

	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
	Entity _activePerspective;
};
#endif