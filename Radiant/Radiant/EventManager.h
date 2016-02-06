#ifndef _EVENTMANAGER_H_
#define _EVENTMANAGER_H_

#pragma once
#include "Entity.h"
#include "OverlayManager.h"

struct Events
{
	OverlayData* overlay;
	std::function<void()> leftClick;
};


class EventManager
{
public:
	EventManager(OverlayManager& overlay);
	~EventManager();

	const void BindLeftClick(const Entity& entity, std::function<void()> callback);


	const void DoEvents()const;


private:
	const void _DoLeftClick(const Events& e)const;
	const bool _Hovering(const OverlayData* overlay)const;
private:
	std::vector<Events> _events;
	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
	OverlayData _standard;
};

#endif