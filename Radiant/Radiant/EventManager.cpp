#include "EventManager.h"
#include "System.h"
#include "Utils.h"


EventManager::EventManager(OverlayManager& manager)
{
	manager.SendOverlayDataPointer([this](const Entity& entity, OverlayData* data)
	{
		auto indexIt = _entityToOverlay.find(entity);
		if (indexIt != _entityToOverlay.end())
		{
			_entityToOverlay[entity]->overlay = data;
		}
		else
		{
			OverlayEvents* e = nullptr;
			try { e = new OverlayEvents; }
			catch (std::exception& ex) { ex; TraceDebug("Failed to create overlayevent in lambda."); SAFE_DELETE(e); return; }
			e->overlay = data;
			e->hovering = false;
			e->checkE = true;
			_entityToOverlay[entity] = e;
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
	std::vector<Entity> v;
	for (auto& o : _entityToOverlay)
		v.push_back(std::move(o.first));

	for (auto& o : _entityToObject)
		v.push_back(std::move(o.first));

	for (auto& o : v)
		ReleaseEvents(o);
}

const void EventManager::BindEventToEntity(const Entity & entity,const Type& type)
{
	switch (type)
	{
	case EventManager::Type::Overlay:
	{
		auto indexIt = _entityToOverlay.find(entity);

		if (indexIt != _entityToOverlay.end())
		{
			TraceDebug("Tried to bind event handler to an entity that already had one.");
			return;
		}
		OverlayEvents* e = nullptr;
		try { e = new OverlayEvents; }
		catch (std::exception&ex) { ex; TraceDebug("Failed to create overlayevent."); SAFE_DELETE(e); return; }
		e->overlay = &_standard;
		e->checkE = true;
		e->hovering = false;
		_entityToOverlay[entity] = e;
		break;
	}
	case EventManager::Type::Object:
	{
		auto indexIt = _entityToObject.find(entity);

		if (indexIt != _entityToObject.end())
		{
			TraceDebug("Tried to bind event handler to an entity that already had one.");
			return;
		}
		ObjectEvents* e = nullptr;
		try { e = new ObjectEvents; }
		catch (std::exception&ex) { ex; TraceDebug("Failed to create overlayevent."); SAFE_DELETE(e); return; }
		e->checkE = true;
		_entityToObject[entity] = e;
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
	auto indexIt = _entityToOverlay.find(entity);

	if (indexIt != _entityToOverlay.end())
	{
		indexIt->second->checkE = active;
		return;
	}
	auto indexIt2 = _entityToObject.find(entity);

	if (indexIt2 != _entityToObject.end())
	{
		indexIt->second->checkE = active;
	}
	TraceDebug("Tried to set check event value to an entity that had no event handler.");
}

const void EventManager::ReleaseEvents(const Entity & entity)
{
	auto got = _entityToOverlay.find(entity);

	if (got != _entityToOverlay.end())
	{
		SAFE_DELETE(got->second);
		_entityToOverlay.erase(entity);

		return;
	}


	auto got2 = _entityToObject.find(entity);

	if (got2 != _entityToObject.end())
	{
		SAFE_DELETE(got2->second);
		_entityToObject.erase(entity);
		return;
	}



	TraceDebug("Tried to release nonexistant entity %d from EventManager.\n", entity.ID);
}

const void EventManager::_BindLeftClick(const Entity& entity, std::function<void()> callback)
{
	auto indexIt = _entityToOverlay.find(entity);

	if (indexIt != _entityToOverlay.end())
	{
		indexIt->second->leftClick = std::move(callback);
		return;
		
	}

}

const void EventManager::_BindOnEnter(const Entity & entity, std::function<void()> callback)
{
	auto indexIt = _entityToOverlay.find(entity);

	if (indexIt != _entityToOverlay.end())
	{
		indexIt->second->onEnter = std::move(callback);
		return;
	}
	return void();
}

const void EventManager::_BindOnExit(const Entity & entity, std::function<void()> callback)
{
	auto indexIt = _entityToOverlay.find(entity);

	if (indexIt != _entityToOverlay.end())
	{
		indexIt->second->onExit = std::move(callback);
		return;
	}
	return void();
}

const void EventManager::_BindUpdate(const Entity & entity, std::function<void()> callback)
{
	auto indexIt = _entityToOverlay.find(entity);

	if (indexIt != _entityToOverlay.end())
	{
		indexIt->second->update = std::move(callback);
		return;
	}


	auto indexIt2 = _entityToObject.find(entity);

	if (indexIt2 != _entityToObject.end())
	{
		indexIt->second->update = std::move(callback);
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
	for (auto& e : _entityToOverlay)
	{
		if (e.second->checkE)
		{
			// Do hovering events for all.
			bool yes = false;
			if (posX >= e.second->overlay->posX)
			{
				if (posY >= e.second->overlay->posY)
				{
					if (posX <= e.second->overlay->width + e.second->overlay->posX)
					{
						if (posY <= e.second->overlay->height + e.second->overlay->posY)
						{
							yes = true;
							if (!e.second->hovering)
							{

								e.second->hovering = true;
								if (e.second->onEnter)
									e.second->onEnter();
							}
						}
					}
				}
			}
			if (!yes && e.second->hovering)
			{
				e.second->hovering = false;
				if (e.second->onExit)
					e.second->onExit();
			}


			// Do the update event
			if (e.second->update)
				e.second->update();


			// Do clicking event.
			if (e.second->leftClick)
				if (lclick)
					if (e.second->hovering)
						e.second->leftClick();
		}
	}


	for (auto& e : _entityToObject)
	{
		if (e.second->checkE)
		{
			// Do the update event
			if (e.second->update)
				e.second->update();
		}
	}



}
