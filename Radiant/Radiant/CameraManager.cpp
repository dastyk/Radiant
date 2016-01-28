#include "CameraManager.h"
#include "System.h"


CameraManager::CameraManager()
{
}


CameraManager::~CameraManager()
{

}

const void CameraManager::CreateCamera(Entity entity, const char * filename, bool usePerspective)
{
	Options* o = System::GetOptions();
	CameraData cData(entity);
	cData.aspect = static_cast<float>(System::GetWindowHandler()->GetWindowHeight()) / static_cast<float>(System::GetWindowHandler()->GetWindowWidth());
	cData.fov = o->GetFoV();
	cData.farp = o->GetViewDistance();
	cData.usePerspective = usePerspective;

	_entityToIndex[entity] = static_cast<int>(_cameras.size());
	_cameras.push_back(move(cData));
	return void();
}

const void CameraManager::GatherCameras(std::function<void(CameraMatrixes&)> ProvideCamera)
{
	CameraMatrixes cam;
	for (auto c : _cameras)
	{
		if (c.usePerspective)
		{
			cam.viewMatrix = _GetViewMatrix(c.OwningEntity);
			cam.projectionMatrix = _GetProjectionMatrix(c.OwningEntity);
			cam.viewProjectionMatrix = _GetViewProjectionMatrix(c.OwningEntity);
			ProvideCamera(cam);
		}
	}

	return void();
}

const void CameraManager::TransformChanged(Entity entity, const DirectX::XMFLOAT3& pos)
{
	auto camIt = _entityToIndex.find( entity );

	if ( camIt != _entityToIndex.end() )
	{
		CameraData& cam = _cameras.at(camIt->second);
		// The entity has a camera (we have an entry here)
		DirectX::XMStoreFloat4x4(&cam.viewMatrix, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&pos), cam.lookAt, cam.up));
		DirectX::XMStoreFloat4x4(&cam.projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(cam.fov, cam.aspect, cam.nearp, cam.farp));
		DirectX::XMStoreFloat4x4(&cam.viewProjectionMatrix, DirectX::XMLoadFloat4x4(&cam.viewMatrix) * DirectX::XMLoadFloat4x4(&cam.projectionMatrix));
	}
	return void();
}

DirectX::XMFLOAT4X4 CameraManager::_GetViewMatrix(Entity entity)
{
	return DirectX::XMFLOAT4X4();
}

DirectX::XMFLOAT4X4 CameraManager::_GetProjectionMatrix(Entity entity)
{
	return DirectX::XMFLOAT4X4();
}

DirectX::XMFLOAT4X4 CameraManager::_GetViewProjectionMatrix(Entity entity)
{
	return DirectX::XMFLOAT4X4();
}
