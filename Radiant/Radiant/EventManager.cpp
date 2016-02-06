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
			Events e;
			e.overlay = data;
			_entityToIndex[entity] = static_cast<int>(_events.size());
			_events.push_back(e);
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
		Events e;
		e.overlay = &_standard;
		e.leftClick = std::move(callback);
		_entityToIndex[entity] = static_cast<int>(_events.size());
		_events.push_back(std::move(e));
		return;
	}

	return void();
}

const void EventManager::DoEvents() const
{
	auto i = System::GetInput();
	if (i->GetMouseKeyStateAndReset(VK_LBUTTON))
		for_each(_events.begin(), _events.end(), [this](const Events& e) {_DoLeftClick(e);});
}

const void EventManager::_DoLeftClick(const Events& e) const
{
	if (e.leftClick)
	{
		if (_Hovering(e.overlay))
		{
			e.leftClick();
		}
	}
}


const bool EventManager::_Hovering(const OverlayData* overlay) const
{
	auto i = System::GetInput();
	int posX, posY;
	i->GetMousePos(posX, posY);
	if (posX >= overlay->posX)
	{
		if (posY >= overlay->posY)
		{
			if (posX <= overlay->width + overlay->posX)
			{
				if (posY <= overlay->height + overlay->posY)
				{
					return true;
				}
			}
		}
	}
	return false;
}
