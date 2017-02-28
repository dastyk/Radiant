#include "CameraManager.h"
#include "System.h"
#include "Utils.h"

using namespace DirectX;

CameraManager::CameraManager(TransformManager& transformManager)
{
	System::GetGraphics()->AddCameraProvider(this);

	transformManager.TransformChanged += Delegate<void( const Entity&, const XMMATRIX&, const XMVECTOR&, const XMVECTOR&, const XMVECTOR& )>::Make<CameraManager, &CameraManager::_TransformChanged>( this );
	
	Options* o = System::GetOptions();

	_default.aspect = static_cast<float>(o->GetScreenResolutionWidth()) / static_cast<float>(o->GetScreenResolutionHeight());
	_default.fov = (float)XMConvertToRadians((float)o->GetFoV());
	_default.farp = (float)o->GetViewDistance();
	_default.nearp = o->GetNearPlane();
	_default.viewDistance = _default.farp;
	DirectX::XMStoreFloat4x4(&_default.projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(_default.fov, _default.aspect, _default.nearp, _default.farp));
	_activePerspective = &_default;
}


CameraManager::~CameraManager()
{
	std::vector<Entity> v;
	for (auto& o : _entityToCamera)
		v.push_back(std::move(o.first));

	for (auto& o : v)
		ReleaseCamera(o);
}

const void CameraManager::CreateCamera(const Entity& entity)
{
	auto indexIt = _entityToCamera.find(entity);

	if (indexIt != _entityToCamera.end())
	{
		return;
	}

	Options* o = System::GetOptions();
	CameraData* cData = nullptr;
	try { cData = new CameraData; }
	catch (std::exception& e) { e;SAFE_DELETE(cData); throw ErrorMsg(1800001, L"Failed to create camera."); }
	cData->aspect = static_cast<float>(o->GetScreenResolutionWidth()) / static_cast<float>(o->GetScreenResolutionHeight());
	cData->fov = (float)XMConvertToRadians( (float)o->GetFoV() );
	cData->farp = (float)o->GetViewDistance();
	cData->viewDistance = cData->farp;
	cData->nearp = o->GetNearPlane();
	DirectX::XMStoreFloat4x4(&cData->projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(cData->fov, cData->aspect, cData->nearp, cData->farp));

	_entityToCamera[entity] = cData;
	return void();
}

const void CameraManager::SetActivePerspective(const Entity& entity)
{
	auto cameraIt = _entityToCamera.find(entity);
	if (cameraIt != _entityToCamera.end())
	{
		_activePerspective = _entityToCamera[entity];
		cameraChanged(_activePerspective->camPos);
	}
	return void();
}

const void CameraManager::SetDrawDistance(const Entity & entity, float dist)
{
	auto cameraIt = _entityToCamera.find(entity);
	if (cameraIt != _entityToCamera.end())
	{
		_entityToCamera[entity]->farp = dist;
		DirectX::XMStoreFloat4x4(&_entityToCamera[entity]->projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(_entityToCamera[entity]->fov, _entityToCamera[entity]->aspect, _entityToCamera[entity]->nearp, _entityToCamera[entity]->farp));

	}
}

const void CameraManager::SetViewDistance(const Entity & entity, float dist)
{
	auto cameraIt = _entityToCamera.find(entity);
	if (cameraIt != _entityToCamera.end())
	{
		_entityToCamera[entity]->viewDistance = dist;
	}
}

const void CameraManager::ReleaseCamera(const Entity & entity)
{
	auto cameraIt = _entityToCamera.find(entity);
	if (cameraIt != _entityToCamera.end())
	{
		if (_activePerspective = cameraIt->second)
			_activePerspective = &_default;

		SAFE_DELETE(cameraIt->second);
		_entityToCamera.erase(cameraIt->first);
	}
	return void();
}

void CameraManager::GatherCam(CameraData*& Cam)
{
	Cam = _activePerspective;
}

const void CameraManager::BindToRenderer(bool exclusive)
{
	if (exclusive)
		System::GetGraphics()->ClearCameraProviders();
	System::GetGraphics()->AddCameraProvider(this);
	return void();
}

const DirectX::BoundingFrustum CameraManager::GetFrustum(const Entity & entity)
{
	auto cameraIt = _entityToCamera.find(entity);
	if (cameraIt != _entityToCamera.end())
	{
		DirectX::BoundingFrustum f;

		DirectX::BoundingFrustum::CreateFromMatrix(f, DirectX::XMLoadFloat4x4( &cameraIt->second->projectionMatrix));

		f.Transform(f, DirectX::XMMatrixInverse(nullptr,DirectX::XMLoadFloat4x4(&cameraIt->second->viewMatrix)));

		return f;
	}
	
	return DirectX::BoundingFrustum();
}

void CameraManager::_TransformChanged( const Entity& entity, const XMMATRIX& tran, const XMVECTOR& pos, const XMVECTOR& dir, const XMVECTOR& up )
{
	auto cameraIt = _entityToCamera.find(entity);
	if (cameraIt != _entityToCamera.end())
	{
		// The entity has a camera (we have an entry here)
		CameraData* d = cameraIt->second;

		XMStoreFloat3(&d->camPos, pos);

		XMVECTOR lookAt;
		XMMATRIX rotationMatrix;

		// Translate the rotated camera position to the location of the viewer.
		lookAt = XMVectorAdd(pos, dir);
		
		// Finally create the view matrix from the three updated vectors.
		XMMATRIX viewMatrix = XMMatrixLookAtLH(pos, lookAt, up);

		DirectX::XMStoreFloat4x4(&d->viewMatrix, viewMatrix);

		DirectX::XMStoreFloat4x4(&d->viewProjectionMatrix, viewMatrix * DirectX::XMLoadFloat4x4(&d->projectionMatrix));

		cameraChanged(d->camPos);
	}
	return void();
}
