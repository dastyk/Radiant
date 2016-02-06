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

const void EventManager::BindEventToOverlay(const Entity & entity)
{
	auto indexIt = _entityToOverlayIndex.find(entity);

	if (indexIt != _entityToOverlayIndex.end())
	{
		TraceDebug("Tried to bind event handler to an entity that already had one.");
		return;
	}
	OverlayEvents e;
	e.overlay = &_standard;
	_entityToOverlayIndex[entity] = static_cast<int>(_overlayEvents.size());
	_overlayEvents.push_back(std::move(e));
	return void();
}

const void EventManager::BindLeftClick(const Entity& entity, std::function<void()> callback)
{
	auto indexIt = _entityToOverlayIndex.find(entity);

	if (indexIt != _entityToOverlayIndex.end())
	{
		_overlayEvents[indexIt->second].leftClick = std::move(callback);
		return;
		
	}
	TraceDebug("Tried to bind event to an entity that had no event handler.");
	return void();
}

const void EventManager::BindOnEnter(const Entity & entity, std::function<void()> callback)
{
	auto indexIt = _entityToOverlayIndex.find(entity);

	if (indexIt != _entityToOverlayIndex.end())
	{
		_overlayEvents[indexIt->second].onEnter = std::move(callback);
		return;
	}
	TraceDebug("Tried to bind event to an entity that had no event handler.");
	return void();
}

const void EventManager::BindOnExit(const Entity & entity, std::function<void()> callback)
{
	auto indexIt = _entityToOverlayIndex.find(entity);

	if (indexIt != _entityToOverlayIndex.end())
	{
		_overlayEvents[indexIt->second].onExit = std::move(callback);
		return;
	}
	TraceDebug("Tried to bind event to an entity that had no event handler.");
	return void();
}

const void EventManager::BindUpdate(const Entity & entity, std::function<void()> callback)
{
	auto indexIt = _entityToOverlayIndex.find(entity);

	if (indexIt != _entityToOverlayIndex.end())
	{
		_overlayEvents[indexIt->second].update = std::move(callback);
		return;
	}
	TraceDebug("Tried to bind event to an entity that had no event handler.");
	return void();
}

const void EventManager::DoEvents()
{
	auto i = System::GetInput();

	int posX, posY;
	i->GetMousePos(posX, posY);
	bool lclick = i->GetMouseKeyStateAndReset(VK_LBUTTON);
	// Call the events for each entity.
	for_each(_overlayEvents.begin(), _overlayEvents.end(), [posX, posY, lclick](OverlayEvents& e)
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

	});




	

}
