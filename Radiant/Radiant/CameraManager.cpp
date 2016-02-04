#include "CameraManager.h"
#include "System.h"
//#include "Utils.h"

CameraManager::CameraManager(TransformManager& transformManager) : _graphics(*System::GetGraphics())
{
	_graphics.AddCameraProvider(this);
	transformManager.SetTransformChangeCallback2([this](Entity entity, const DirectX::XMVECTOR & pos, const DirectX::XMVECTOR & lookAt, const DirectX::XMVECTOR & up)
	{
		TransformChanged(entity, pos, lookAt, up);
	});
}


CameraManager::~CameraManager()
{

}

const void CameraManager::CreateCamera(Entity entity)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		TraceDebug("Tried to bind camera component to entity that already had one.");
		return;
	}

	Options* o = System::GetOptions();
	CameraData cData(entity);
	cData.aspect = static_cast<float>(System::GetWindowHandler()->GetWindowWidth()) / static_cast<float>(System::GetWindowHandler()->GetWindowHeight());
	cData.fov = (float)XMConvertToRadians( (float)o->GetFoV() );
	cData.farp = (float)o->GetViewDistance();
	DirectX::XMStoreFloat4x4(&cData.projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(cData.fov, cData.aspect, cData.nearp, cData.farp));

	_entityToIndex[entity] = static_cast<int>(_cameras.size());
	_cameras.push_back(move(cData));
	return void();
}

const void CameraManager::SetActivePerspective(Entity& entity)
{
	auto cameraIt = _entityToIndex.find(entity);
	if (cameraIt != _entityToIndex.end())
	{
		// The entity has a camera (we have an entry here)
		_activePerspective = entity;
	}
	return void();
}

void CameraManager::GatherCam(CamData & Cam)
{
	auto cameraIt = _entityToIndex.find(_activePerspective);
	if (cameraIt != _entityToIndex.end())
	{
		Cam.viewMatrix = _cameras[cameraIt->second].viewMatrix;
		Cam.projectionMatrix = _cameras[cameraIt->second].projectionMatrix;
		Cam.viewProjectionMatrix = _cameras[cameraIt->second].viewProjectionMatrix;
	}
	else
	{
		DirectX::XMStoreFloat4x4(&Cam.viewMatrix, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&Cam.projectionMatrix, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&Cam.viewProjectionMatrix, DirectX::XMMatrixIdentity());
	}
}

const void CameraManager::BindToRenderer(bool exclusive)
{
	if (exclusive)
		System::GetGraphics()->ClearCameraProviders();
	System::GetGraphics()->AddCameraProvider(this);
	return void();
}

//void CameraManager::GatherCam(std::function<void(CamData&)> ProvideCam)
//{
//	auto cameraIt = _entityToIndex.find(_activePerspective);
//	if (cameraIt != _entityToIndex.end())
//	{
//		CamData data;
//		data.viewMatrix = _cameras[cameraIt->second].viewMatrix;
//		data.projectionMatrix = _cameras[cameraIt->second].projectionMatrix;
//		data.viewProjectionMatrix = _cameras[cameraIt->second].viewProjectionMatrix;
//
//		ProvideCam(data);
//	}
//	else
//	{
//		CamData data;
//		DirectX::XMStoreFloat4x4(&data.viewMatrix, DirectX::XMMatrixIdentity());
//		DirectX::XMStoreFloat4x4(&data.projectionMatrix, DirectX::XMMatrixIdentity());
//		DirectX::XMStoreFloat4x4(&data.viewProjectionMatrix, DirectX::XMMatrixIdentity());
//
//		ProvideCam(data);
//	}
//}


const void CameraManager::TransformChanged(Entity entity, const DirectX::XMVECTOR & pos, const DirectX::XMVECTOR & dir, const DirectX::XMVECTOR & up)
{
	auto cameraIt = _entityToIndex.find(entity);
	if (cameraIt != _entityToIndex.end())
	{
		// The entity has a camera (we have an entry here)
		CameraData& d = _cameras[cameraIt->second];



		XMVECTOR lookAt;
		XMMATRIX rotationMatrix;

		// Translate the rotated camera position to the location of the viewer.
		lookAt = XMVectorAdd(pos, dir);

		// Finally create the view matrix from the three updated vectors.
		XMMATRIX viewMatrix = XMMatrixLookAtLH(pos, lookAt, up);

		DirectX::XMStoreFloat4x4(&d.viewMatrix, viewMatrix);

		DirectX::XMStoreFloat4x4(&d.viewProjectionMatrix, viewMatrix * DirectX::XMLoadFloat4x4(&d.projectionMatrix));
	}
	
	return void();
}
