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
#include "MaterialManager.h"

using namespace DirectX;

class OverlayManager : public IOverlayProvider
{
public:
	OverlayManager(TransformManager& transformManager, MaterialManager& materialManager);
	~OverlayManager();

	
	void GatherOverlayJobs(std::function<void(OverlayData&)> ProvideJob);
	const void CreateOverlay(const Entity& entity);
	const void SetExtents(const Entity& entity, float width, float height);
private:
	struct Overlays
	{
		Entity OwningEntity;

		float width;
		float height;

		float posX;
		float posY;
		float posZ;

		ShaderData Material;
	};

private:
	const void TransformChanged(const Entity& entity, const DirectX::XMVECTOR & pos);
	const void MaterialChanged(const Entity& entity, const ShaderData& material);
private:

	Graphics& _graphics;
	std::vector<Overlays> _overlays;

	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
};
#endif