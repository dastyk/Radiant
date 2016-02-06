#include "EventManager.h"

#include "System.h"

EventManager::EventManager(OverlayManager& manager)
{
	manager.SendOverlayDataPointer([this](const Entity& entity, OverlayData* data)
	{
		auto indexIt = _entityToIndex.find(entity);
		if (indexIt != _entityToIndex.end())
		{
			_events[_entityToIndex[entity]].overlay = data;
		}
		else
		{
			OverlayEvents e;
			e.overlay = data;
			e.hovering = false;
			_entityToIndex[entity] = static_cast<int>(_events.size());
			_events.push_back(std::move(e));
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

const void EventManager::BindLeftClick(const Entity& entity, std::function<void()> callback)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		_events[indexIt->second].leftClick = std::move(callback);
		return;
		
	}
	else
	{
		OverlayEvents e;
		e.overlay = &_standard;
		e.leftClick = std::move(callback);
		_entityToIndex[entity] = static_cast<int>(_events.size());
		_events.push_back(std::move(e));
		return;
	}

	return void();
}

const void EventManager::BindOnEnter(const Entity & entity, std::function<void()> callback)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		_events[indexIt->second].onEnter = std::move(callback);
		return;
	}
	else
	{
		OverlayEvents e;
		e.overlay = &_standard;
		e.onEnter = std::move(callback);
		_entityToIndex[entity] = static_cast<int>(_events.size());
		_events.push_back(std::move(e));
		return;
	}

	return void();
}

const void EventManager::BindOnExit(const Entity & entity, std::function<void()> callback)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		_events[indexIt->second].onExit = std::move(callback);
		return;
	}
	else
	{
		OverlayEvents e;
		e.overlay = &_standard;
		e.onExit = std::move(callback);
		_entityToIndex[entity] = static_cast<int>(_events.size());
		_events.push_back(std::move(e));
		return;
	}

	return void();
}

const void EventManager::DoEvents()
{
	auto i = System::GetInput();

	int posX, posY;
	i->GetMousePos(posX, posY);

	// Do hovering events for all.
	for_each(_events.begin(), _events.end(), [posX, posY](OverlayEvents& e)
	{
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
	
	});

	// Do clicking events for all.
	if (i->GetMouseKeyStateAndReset(VK_LBUTTON))
		for_each(_events.begin(), _events.end(), [](const OverlayEvents& e)
	{	
		if (e.leftClick)
			if (e.hovering)
				e.leftClick();
	});
}
