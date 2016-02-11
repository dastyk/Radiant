#ifndef _EVENTMANAGER_H_
#define _EVENTMANAGER_H_

#pragma once
#include "Entity.h"
#include "OverlayManager.h"



class EventManager
{
public:
	enum class Type : unsigned
	{
		Overlay = 0U,
		Object = 1U
	};
	enum class EventType : unsigned
	{
		LeftClick = 0U,
		OnEnter = 1U,
		OnExit = 2U,
		Update = 3U
	};
	struct OverlayEvents
	{
		bool hovering;
		const OverlayData* overlay;
		std::function<void()> leftClick;
		std::function<void()> onEnter;
		std::function<void()> onExit;
		std::function<void()> update;
		bool checkE;

	};
	struct ObjectEvents
	{
		//bool hovering;
		//const OverlayData* overlay;
		//std::function<void()> leftClick;
		//std::function<void()> onEnter;
		//std::function<void()> onExit;
		std::function<void()> update;
		bool checkE;
	};
public:
	EventManager(OverlayManager& overlay);
	~EventManager();

	const void BindEventToEntity(const Entity& entity,const Type& type);
	const void BindEvent(const Entity& entity, EventType type, std::function<void()> callback);
	const void ToggleEventCalls(const Entity& entity, bool active);
	const void ReleaseEvents(const Entity& entity);
	const void DoEvents();


private:
	const void _BindLeftClick(const Entity& entity, std::function<void()> callback);
	const void _BindOnEnter(const Entity& entity, std::function<void()> callback);
	const void _BindOnExit(const Entity& entity, std::function<void()> callback);
	const void _BindUpdate(const Entity & entity, std::function<void()> callback);
private:

	std::unordered_map<Entity, OverlayEvents*, EntityHasher> _entityToOverlay;

	std::unordered_map<Entity, ObjectEvents*, EntityHasher> _entityToObject;
	OverlayData _standard;
};

#endif