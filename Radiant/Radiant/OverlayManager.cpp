#include "OverlayManager.h"
#include "System.h"


OverlayManager::OverlayManager(TransformManager& transformManager) : _graphics(*System::GetGraphics())
{
	_graphics.AddOverlayProvider(this);
	_transformManager = &transformManager;
	//transformManager.SetTransformChangeCallback2([this](Entity entity, const DirectX::XMVECTOR & pos, const DirectX::XMVECTOR & lookAt, const DirectX::XMVECTOR & up)
	//{
	//	TransformChanged(entity, pos, lookAt, up);
	//});
}


OverlayManager::~OverlayManager()
{
}



void OverlayManager::GatherOverlayJobs(std::function<void(OverlayData&)> ProvideJob)
{
}

const void OverlayManager::CreateOverlay(Entity entity)
{
	return void();
}

const void OverlayManager::TransformChanged(Entity entity, const DirectX::XMVECTOR & pos)
{
	return void();
}
