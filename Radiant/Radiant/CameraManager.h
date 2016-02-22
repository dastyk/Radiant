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

	const void CreateCamera(const Entity& entity);

	const void SetActivePerspective(const Entity& entity);
	const void ReleaseCamera(const Entity& entity);
	void GatherCam(CameraData*& Cam);

	const void BindToRenderer(bool exclusive);

	const DirectX::BoundingFrustum GetFrustum(const Entity& entity);

private:


private:
	void _TransformChanged( const Entity& entity, const DirectX::XMMATRIX& tran, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& up );

private:
	std::unordered_map<Entity, CameraData*, EntityHasher> _entityToCamera;
	CameraData* _activePerspective;
	CameraData _default;
};
#endif