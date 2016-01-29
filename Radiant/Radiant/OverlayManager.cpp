#include "OverlayManager.h"
#include "System.h"


OverlayManager::OverlayManager(TransformManager& transformManager) : _graphics(*System::GetGraphics())
{
	_graphics.AddOverlayProvider(this);
	_transformManager = &transformManager;
	transformManager.SetTransformChangeCallback3([this](Entity entity, const DirectX::XMVECTOR & pos)
	{
		TransformChanged(entity, pos);
	});
}


OverlayManager::~OverlayManager()
{
}



void OverlayManager::GatherOverlayJobs(std::function<void(OverlayData&)> ProvideJob)
{
}

const void OverlayManager::CreateOverlay(Entity& entity)
{
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		return;
	}

	Overlays data;
	data.OwningEntity = entity;
	data.height = 0;
	data.width = 0;
	data.posX = 0;
	data.posY = 0;

	_entityToIndex[entity] = static_cast<int>(_overlays.size());
	_overlays.push_back(move(data));

	return void();
}

const void OverlayManager::SetExtents(Entity & entity, uint width, uint height)
{
	return void();
}

const void OverlayManager::TransformChanged(Entity entity, const DirectX::XMVECTOR & pos)
{
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		_overlays[indexIt->second].posX = XMVectorGetX(pos);
		_overlays[indexIt->second].posY = XMVectorGetY(pos);
	}
	return void();
}
