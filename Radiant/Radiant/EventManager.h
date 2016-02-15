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
		Overlay = 1U << 0U,
		Object = 1U << 1U
	};
	enum class EventType : unsigned
	{
		LeftClick = 1U << 0U,
		OnEnter = 1U << 1U,
		OnExit = 1U << 2U,
		Update = 1U << 3U,
		Drag = 1U << 5U
	};
	struct OverlayEvents
	{
		bool hovering;
		bool dragged;
		const OverlayData* overlay;
		std::function<void()> leftClick;
		std::function<void()> onEnter;
		std::function<void()> onExit;
		std::function<void()> update;
		std::function<void()> drag;
		bool checkE;
		bool del;
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
		bool del;
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
	const void _BindDrag(const Entity& entity, std::function<void()> callback);
	const void _ReleaseEvents();
	const void _ReleaseEvents(const Entity& entity);
	const void _CreateEventHandlers();
private:

	std::unordered_map<Entity, OverlayEvents*, EntityHasher> _entityToOverlay;

	std::unordered_map<Entity, ObjectEvents*, EntityHasher> _entityToObject;

	struct EC
	{
		Entity e;
		Type t;
		EC(const Entity& e, const Type& t) :e(e), t(t)
		{

		}
	};
	struct DA
	{
		Entity e;
		const OverlayData* data;
		DA(const Entity& e, const OverlayData* d) :e(e), data(d)
		{

		}
	};
	struct EA
	{
		Entity e;
		EventType type;
		std::function<void()> c;
		EA(const Entity& e,const EventType& type, std::function<void()> c ) :e(e), type(type), c(std::move(c))
		{

		}
	};
	struct EVC
	{
		bool call;
		Entity e;
		EVC(bool call, const Entity& e) :call(call), e(e)
		{

		}
	};
	std::vector<EC> _toCreate;
	std::vector<DA> _toAdd;
	std::vector<EA> _toAddE;
	std::vector<EVC> _toEVC;

	OverlayData _standard;
};

#endif