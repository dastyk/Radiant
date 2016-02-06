#ifndef _EVENTMANAGER_H_
#define _EVENTMANAGER_H_

#pragma once
#include "Entity.h"
#include "OverlayManager.h"

struct OverlayEvents
{
	bool hovering;
	const OverlayData* overlay;
	std::function<void()> leftClick;
	std::function<void()> onEnter;
	std::function<void()> onExit;
	std::function<void()> update;

};
struct ObjectEvents
{
	//bool hovering;
	//const OverlayData* overlay;
	//std::function<void()> leftClick;
	//std::function<void()> onEnter;
	//std::function<void()> onExit;
	std::function<void()> update;

};

class EventManager
{
public:
	EventManager(OverlayManager& overlay);
	~EventManager();

	const void BindEventToOverlay(const Entity& entity);
	const void BindLeftClick(const Entity& entity, std::function<void()> callback);
	const void BindOnEnter(const Entity& entity, std::function<void()> callback);
	const void BindOnExit(const Entity& entity, std::function<void()> callback);
	const void BindUpdate(const Entity & entity, std::function<void()> callback);

	const void DoEvents();


private:
//	const void _DoLeftClick(const Events& e)const;
//	const void _Hovering(Events& e)const;
private:
	std::vector<OverlayEvents> _overlayEvents;
	std::unordered_map<Entity, unsigned, EntityHasher> _entityToOverlayIndex;

	std::vector<ObjectEvents> _objectEvents;
	std::unordered_map<Entity, unsigned, EntityHasher> _entityToObjectIndex;
	OverlayData _standard;
};

#endif