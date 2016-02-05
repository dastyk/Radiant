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

	std::function<void()> callback;
};


class ClickingManager
{
public:
	ClickingManager(TransformManager& trans, OverlayManager& overlay);
	~ClickingManager();

	const void BindOverlay(Entity& entity, std::function<void()> callback);


	const void DoClick(Entity& entity)const;


private:
	const void _IsClicked(const ClickableOverlay& overlay)const;
	const void _TransformChanged(const Entity& entity, const DirectX::XMVECTOR& pos);
	const void _ExtentChanged(const Entity& entity, const float width, const float height);
private:
	std::vector<ClickableOverlay> _cOverlays;

	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
};

#endif