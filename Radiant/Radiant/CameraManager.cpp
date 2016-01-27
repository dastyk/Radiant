#include "CameraManager.h"
#include "System.h"


CameraManager::CameraManager()
{
}


CameraManager::~CameraManager()
{

}

const void CameraManager::CreateCamera(Entity entity, const char * filename)
{
	Options* o = System::GetOptions();
	CameraData cData(entity);
	cData.aspect = static_cast<float>(System::GetWindowHandler()->GetWindowHeight()) / static_cast<float>(System::GetWindowHandler()->GetWindowWidth());
	cData.fov = o->GetFoV();
	cData.farp = o->GetViewDistance();

	_entityToIndex[entity] = static_cast<int>(_cameras.size());
	_cameras.push_back(move(cData));
	return void();
}

const void CameraManager::TransformChanged(Entity entity, const DirectX::XMMATRIX & transform)
{
	return void();
}
