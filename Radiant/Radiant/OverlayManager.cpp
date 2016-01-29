#include "OverlayManager.h"
#include "System.h"


OverlayManager::OverlayManager(TransformManager& transformManager) : _graphics(*System::GetGraphics())
{
	// Add the manager to the graphics
	_graphics.AddOverlayProvider(this);

	// Set the callback function
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
	OverlayData data;
	for (auto o : _overlays)
	{
		data.height = o.height;
		data.width = o.width;
		data.posX = o.posX;
		data.posY = o.posY;
		data.posZ = o.posZ;
	}

}

const void OverlayManager::CreateOverlay(Entity& entity)
{
	// Chech if entity already has an overlay.
	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		return;
	}


	// Create new overlay and bind it to the entity.
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

const void OverlayManager::SetExtents(Entity & entity, float width, float height)
{

	auto indexIt = _entityToIndex.find(entity);
	if (indexIt != _entityToIndex.end())
	{
		_overlays[indexIt->second].width = width;
		_overlays[indexIt->second].height = height;
	}
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
