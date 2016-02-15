#include "EventManager.h"
#include "System.h"
#include "Utils.h"


EventManager::EventManager(OverlayManager& manager)
{
	manager.SendOverlayDataPointer([this](const Entity& entity,const OverlayData* data)
	{
		auto indexIt = _entityToOverlay.find(entity);
		if (indexIt != _entityToOverlay.end())
		{
			_entityToOverlay[entity]->overlay = data;
		}
		else
		{
			_toAdd.push_back(std::move(DA(entity, data)));

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
		_ReleaseEvents(o);
}

const void EventManager::BindEventToEntity(const Entity & entity,const Type& type)
{

	_toCreate.push_back(std::move(EC(entity, type)));
	return void();
}

const void EventManager::BindEvent(const Entity & entity, EventType type, std::function<void()> callback)
	{
	_toAddE.push_back(EA(entity, type, std::move(callback)));
}

const void EventManager::ToggleEventCalls(const Entity & entity, bool active)
	{
	_toEVC.push_back(std::move(EVC(active, entity)));
}

const void EventManager::ReleaseEvents(const Entity & entity)
		{
	auto got = _entityToOverlay.find(entity);

	if (got != _entityToOverlay.end())
	{
		got->second->del = true;

			return;
		}


	auto got2 = _entityToObject.find(entity);

	if (got2 != _entityToObject.end())
	{
		got->second->del = true;

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
	auto indexIt2 = _entityToObject.find(entity);

	if (indexIt2 != _entityToObject.end())
	{
		indexIt2->second->leftClick = std::move(callback);
		return;

	}
	TraceDebug("Tried to bind leftclick to an entity that had no event handler.");
	}

const void EventManager::_BindOnEnter(const Entity & entity, std::function<void()> callback)
{
	auto indexIt = _entityToOverlay.find(entity);

	if (indexIt != _entityToOverlay.end())
	{
		indexIt->second->onEnter = std::move(callback);
		return;
	}
	TraceDebug("Tried to bind onenter to an entity that had no event handler.");
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
	TraceDebug("Tried to bind onexit to an entity that had no event handler.");
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

	TraceDebug("Tried to bind update to an entity that had no event handler.");
	return void();
}

const void EventManager::_BindDrag(const Entity & entity, std::function<void()> callback)
{
	auto indexIt = _entityToOverlay.find(entity);

	if (indexIt != _entityToOverlay.end())
	{
		indexIt->second->drag = std::move(callback);
		return;
	}
	
}

const void EventManager::_ReleaseEvents()
{
	std::vector<Entity> v;
	for (auto& o : _entityToOverlay)
		if (o.second->del)
			v.push_back(std::move(o.first));

	for (auto& o : _entityToObject)
		if (o.second->del)
			v.push_back(std::move(o.first));

	for (auto& o : v)
		_ReleaseEvents(o);

}

const void EventManager::_ReleaseEvents(const Entity & entity)
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
}

const void EventManager::_CreateEventHandlers()
{
	if (_toAdd.size())
	{
		for (auto& ta : _toAdd)
		{

			OverlayEvents* e = nullptr;
			try { e = new OverlayEvents; }
			catch (std::exception& ex) { ex; TraceDebug("Failed to create overlayevent in toAdd."); SAFE_DELETE(e); return; }
			e->overlay = ta.data;
			e->hovering = false;
			e->checkE = true;
			e->del = false;
			e->dragged = false;
			_entityToOverlay[ta.e] = e;
		}
		_toAdd.clear();
	}
	if (_toCreate.size())
	{
		for (auto& tc : _toCreate)
		{
			switch (tc.t)
			{
			case EventManager::Type::Overlay:
{
				auto indexIt = _entityToOverlay.find(tc.e);

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
				e->del = false;
				e->dragged = false;
				_entityToOverlay[tc.e] = e;
				break;
}
			case EventManager::Type::Object:
{
				auto indexIt = _entityToObject.find(tc.e);

				if (indexIt != _entityToObject.end())
	{
					TraceDebug("Tried to bind event handler to an entity that already had one.");
		return;
	}
				ObjectEvents* e = nullptr;
				try { e = new ObjectEvents; }
				catch (std::exception&ex) { ex; TraceDebug("Failed to create overlayevent."); SAFE_DELETE(e); return; }
				e->checkE = true;
				e->del = false;
				_entityToObject[tc.e] = e;
				break;
			}
			default:
				TraceDebug("Tried to bind event handler to an entity with unknown type.");
			}
		}
		_toCreate.clear();
	}
	if (_toAddE.size())
	{
		for (auto& te : _toAddE)
		{
			switch (te.type)
			{
			case EventManager::EventType::LeftClick:
				_BindLeftClick(te.e, std::move(te.c));
				break;
			case EventManager::EventType::OnEnter:
				_BindOnEnter(te.e, std::move(te.c));
				break;
			case EventManager::EventType::OnExit:
				_BindOnExit(te.e, std::move(te.c));
				break;
			case EventManager::EventType::Update:
				_BindUpdate(te.e, std::move(te.c));
				break;
			case EventManager::EventType::Drag:
				_BindDrag(te.e, std::move(te.c));
				break;
			default:
				TraceDebug("Unknown EventType");
			}
		}
		_toAddE.clear();
	}

	if (_toEVC.size())
	{
		for (auto& evc : _toEVC)
		{
			auto indexIt = _entityToOverlay.find(evc.e);

			if (indexIt != _entityToOverlay.end())
			{
				indexIt->second->checkE = evc.call;
			}
			auto indexIt2 = _entityToObject.find(evc.e);

			if (indexIt2 != _entityToObject.end())
	{
				indexIt->second->checkE = evc.call;
			}
			TraceDebug("Tried to set check event value to an entity that had no event handler.");
		}
		_toEVC.clear();
	}
}

const void EventManager::DoEvents()
{
	_ReleaseEvents();
	_CreateEventHandlers();
	auto i = System::GetInput();

	int posX, posY;
	i->GetMousePos(posX, posY);
	bool lclick = i->IsMouseKeyPushed(VK_LBUTTON);
	bool ldown = i->IsMouseKeyDown(VK_LBUTTON);
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
				if (e.second->hovering)
				if (lclick)
						e.second->leftClick();

			// Drag event
			if (e.second->drag)
			{
				if (ldown)
				{
					if (e.second->hovering)
						e.second->dragged = true;

					if (e.second->dragged)
						e.second->drag();
				}
				else
					e.second->dragged = false;
			}
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
			if (lclick)
				if (e.second->leftClick)
					e.second->leftClick();
		}

	

}
