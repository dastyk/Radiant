#include "EventManager.h"
#include "System.h"
#include "Utils.h"


EventManager::EventManager(OverlayManager& manager)
{
	manager.SendOverlayDataPointer([this](const Entity& entity, OverlayData* data)
	{
		auto indexIt = _entityToOverlayIndex.find(entity);
		if (indexIt != _entityToOverlayIndex.end())
		{
			_overlayEvents[_entityToOverlayIndex[entity]].overlay = data;
		}
		else
		{
			OverlayEvents e;
			e.overlay = data;
			e.hovering = false;
			e.checkE = true;
			_entityToOverlayIndex[entity] = static_cast<int>(_overlayEvents.size());
			_overlayEvents.push_back(std::move(e));
		}
		
	});
	_standard.height = 0;
	_standard.width = 0;
	_standard.posX = 0;
	_standard.posY = 0;
	_standard.posZ = 0;
	_standard.mat = false;
}

EventManager::~EventManager()
{
}

const void EventManager::BindEventToEntity(const Entity & entity,const Type& type)
{
	switch (type)
	{
	case EventManager::Type::Overlay:
	{
		auto indexIt = _entityToOverlayIndex.find(entity);

		if (indexIt != _entityToOverlayIndex.end())
		{
			TraceDebug("Tried to bind event handler to an entity that already had one.");
			return;
		}
		OverlayEvents e;
		e.overlay = &_standard;
		e.checkE = true;
		e.hovering = false;
		_entityToOverlayIndex[entity] = static_cast<int>(_overlayEvents.size());
		_overlayEvents.push_back(std::move(e));
		break;
	}
	case EventManager::Type::Object:
	{
		auto indexIt = _entityToObjectIndex.find(entity);

		if (indexIt != _entityToObjectIndex.end())
		{
			TraceDebug("Tried to bind event handler to an entity that already had one.");
			return;
		}
		ObjectEvents e;
		e.checkE = true;
		_entityToObjectIndex[entity] = static_cast<int>(_objectEvents.size());
		_objectEvents.push_back(std::move(e));
		break;
	}
	default:
		TraceDebug("Tried to bind event handler to an entity with unknown type.");
	}

	return void();
}

const void EventManager::BindEvent(const Entity & entity, EventType type, std::function<void()> callback)
{
	switch (type)
	{
	case EventManager::EventType::LeftClick:
		_BindLeftClick(entity, callback);
		break;
	case EventManager::EventType::OnEnter:
		_BindOnEnter(entity, callback);
		break;
	case EventManager::EventType::OnExit:
		_BindOnExit(entity, callback);
		break;
	case EventManager::EventType::Update:
		_BindUpdate(entity, callback);
		break;
	default:
		TraceDebug("Tried to bind event to an entity that had no event handler.");
	}
}

const void EventManager::ToggleEventCalls(const Entity & entity, bool active)
{
	auto indexIt = _entityToOverlayIndex.find(entity);

	if (indexIt != _entityToOverlayIndex.end())
	{
		_overlayEvents[indexIt->second].checkE = active;
		return;
	}
	auto indexIt2 = _entityToObjectIndex.find(entity);

	if (indexIt2 != _entityToObjectIndex.end())
	{
		_objectEvents[indexIt2->second].checkE = active;
	}
	TraceDebug("Tried to set check event value to an entity that had no event handler.");
}

const void EventManager::_BindLeftClick(const Entity& entity, std::function<void()> callback)
{
	auto indexIt = _entityToOverlayIndex.find(entity);

	if (indexIt != _entityToOverlayIndex.end())
	{
		_overlayEvents[indexIt->second].leftClick = std::move(callback);
		return;
		
	}
}

const void EventManager::_BindOnEnter(const Entity & entity, std::function<void()> callback)
{
	auto indexIt = _entityToOverlayIndex.find(entity);

	if (indexIt != _entityToOverlayIndex.end())
	{
		_overlayEvents[indexIt->second].onEnter = std::move(callback);
		return;
	}
	return void();
}

const void EventManager::_BindOnExit(const Entity & entity, std::function<void()> callback)
{
	auto indexIt = _entityToOverlayIndex.find(entity);

	if (indexIt != _entityToOverlayIndex.end())
	{
		_overlayEvents[indexIt->second].onExit = std::move(callback);
		return;
	}
	return void();
}

const void EventManager::_BindUpdate(const Entity & entity, std::function<void()> callback)
{
	auto indexIt = _entityToOverlayIndex.find(entity);

	if (indexIt != _entityToOverlayIndex.end())
	{
		_overlayEvents[indexIt->second].update = std::move(callback);
		return;
	}


	auto indexIt2 = _entityToObjectIndex.find(entity);

	if (indexIt2 != _entityToObjectIndex.end())
	{
		_objectEvents[indexIt2->second].update = std::move(callback);
		return;
	}
	return void();
}

const void EventManager::DoEvents()
{
	auto i = System::GetInput();

	int posX, posY;
	i->GetMousePos(posX, posY);
	bool lclick = i->IsMouseKeyPushed(VK_LBUTTON);
	// Call the events for each entity.
	for_each(_overlayEvents.begin(), _overlayEvents.end(), [posX, posY, lclick](OverlayEvents& e)
	{
		if (e.checkE)
		{
			// Do hovering events for all.
			bool yes = false;
			if (posX >= e.overlay->posX)
			{
				if (posY >= e.overlay->posY)
				{
					if (posX <= e.overlay->width + e.overlay->posX)
					{
						if (posY <= e.overlay->height + e.overlay->posY)
						{
							yes = true;
							if (!e.hovering)
							{

								e.hovering = true;
								if (e.onEnter)
									e.onEnter();
							}
						}
					}
				}
			}
			if (!yes && e.hovering)
			{
				e.hovering = false;
				if (e.onExit)
					e.onExit();
			}


			// Do the update event
			if (e.update)
				e.update();


			// Do clicking event.
			if (e.leftClick)
				if (lclick)
					if (e.hovering)
						e.leftClick();
		}
	});


	for_each(_objectEvents.begin(), _objectEvents.end(), [](ObjectEvents& e)
	{
		if (e.checkE)
		{
			// Do the update event
			if (e.update)
				e.update();
		}
	});

	

}
