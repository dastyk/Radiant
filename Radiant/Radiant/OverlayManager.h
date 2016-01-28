#ifndef _OVERLAY_MANAGER_H_
#define _OVERLAY_MANAGER_H_


#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
#include "Entity.h"
#include <map>

//#include "Material.h"
#include "TransformManager.h"
#include "Graphics.h"
#include "IOverlayProvider.h"

using namespace DirectX;

class OverlayManager : public IOverlayProvider
{
public:
	OverlayManager(TransformManager& transformManager);
	~OverlayManager();

	
	void GatherOverlayJobs(std::function<void(OverlayData&)> ProvideJob);
	const void CreateOverlay(Entity entity);

private:
	struct Overlays
	{
		Entity OwningEntity;

		uint width;
		uint height;

		uint posX;
		uint posY;

	};

private:
	const void TransformChanged(Entity entity, const DirectX::XMVECTOR & pos);
private:

	Graphics& _graphics;
	std::vector<Overlays> _overlays;

	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
	TransformManager* _transformManager;
};
#endif