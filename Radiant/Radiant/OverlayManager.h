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

class OverlayManager : public IOverlayProvider
{
public:
	OverlayManager(TransformManager& transformManager, MaterialManager& materialManager);
	~OverlayManager();

	
	void GatherOverlayJobs(std::function<void(OverlayData&)> ProvideJob);
	const void CreateOverlay(const Entity& entity);
	const void SetExtents(const Entity& entity, float width, float height);

	const void BindToRenderer(bool exclusive);


	void SendOverlayDataPointer(std::function<void(const Entity& entity, OverlayData* data)> callback) { _sendOverlayDataPointerCallback = callback; } // eventhandler

private:
	void _TransformChanged(const Entity& entity, const DirectX::XMMATRIX& tran, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& up );
	void _MaterialChanged(const Entity& entity, const ShaderData* material);

private:
	std::function<void(const Entity& entity, OverlayData* data)>  _sendOverlayDataPointerCallback;

	std::vector<OverlayData> _overlays;

	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
};
#endif