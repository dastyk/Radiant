#ifndef _ClickingManager_H_
#define _ClickingManager_H_

#pragma once
#include "Entity.h"
#include "TransformManager.h"
#include "OverlayManager.h"

struct ClickableOverlay
{
	float posX;
	float posY;

	float width;
	float height;


};


class ClickingManager
{
public:
	ClickingManager(TransformManager& trans, OverlayManager& overlay);
	~ClickingManager();

	const void BindOverlay(Entity& entity);


	const bool IsClicked(Entity& entity);

private:
	const void _TransformChanged(const Entity& entity, const DirectX::XMVECTOR& pos);
	const void _ExtentChanged(const Entity& entity, const float width, const float height);
private:
	std::vector<ClickableOverlay> _cOverlays;

	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
};

#endif