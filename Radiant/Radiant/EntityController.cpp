#include "EntityController.h"
#include "System.h"

EntityController::EntityController(EntityManager & e, StaticMeshManager * mesh, TransformManager * trans, CameraManager * cam, MaterialManager * mat, OverlayManager * o, EventManager * _event, LightManager * l, BoundingManager * b, TextManager * text, LightningManager * lightning, DecalManager* decal, AnimationManager* anim)
	: _entity(e), _mesh(mesh), _transform(trans), _camera(cam), _material(mat), _overlay(o), _event(_event), _light(l), _bounding(b), _text(text), _lightning(lightning), _decal(decal), _animation(anim), _popInfo(PopInfo())
{
}

EntityController::~EntityController()
{
	for (auto& l : _listSelections)
		SAFE_DELETE(l.second);
	for (auto& p : _popUps)
		SAFE_DELETE(p.second);
	for (auto& sl : _scrollLists)
		SAFE_DELETE(sl.second);
	for (auto& s : _sliders)
		SAFE_DELETE(s.second);
	for (auto& p : _progressBars)
		SAFE_DELETE(p.second);
}

const void EntityController::ReleaseEntity(const Entity& entity)
{
	_mesh->ReleaseMesh(entity);
	_light->RemoveAreaRectLight(entity);
	_light->RemoveSpotLight(entity);
	_light->RemovePointLight(entity);
	_light->RemoveCapsuleLight(entity);
	_material->ReleaseMaterial(entity);
	_bounding->ReleaseBoundingData(entity);
	_overlay->ReleaseOverlay(entity);
	_text->ReleaseText(entity);
	_event->ReleaseEvents(entity);
	_animation->ReleaseEntity(entity);
	
}

const void EntityController::BindEventHandler(const Entity & entity, const EventManager::Type & type)const
{
	_event->BindEventToEntity(entity, type);
}

const void EntityController::BindEvent(const Entity & entity, const EventManager::EventType & type, std::function<void()> callback)const
{
	_event->BindEvent(entity, type, callback);
	return void();
}


const void EntityController::ToggleVisible(const Entity & entity, bool visible) const
{
	visible ? _mesh->Show(entity, 0) : _mesh->Hide(entity, 0);
	_overlay->ToggleVisible(entity, visible);
	_light->ToggleVisible(entity, visible);
	_text->ToggleVisible(entity, visible);
	return void();
}

const void EntityController::ToggleEventChecking(const Entity & entity, bool active) const
{
	_event->ToggleEventCalls(entity, active);
}

const std::string EntityController::GetValue(const Entity & entity) const
{
	return _text->GetText(entity);
}

const void EntityController::SetProgressBarValue(const Entity & entity, const float value)
{
	auto i = _progressBars.find(entity);
	if (i != _progressBars.end())
	{
		auto& b = i->second;
		float pl = b->width*((b->value - b->minV) / (b->maxV - b->minV));
		b->value = fmin(fmax(value, b->minV), b->maxV);
		float l = b->width*((b->value - b->minV) / (b->maxV - b->minV));
		_animation->PlayAnimation(entity, "scale", l - pl, pl);
	}

	TraceDebug("Tried to set value of progressbar that was not a progressbar");
	return void();
}

const void EntityController::SetProgressBarWidth(const Entity & entity, const float width)
{
	auto i = _progressBars.find(entity);
	if (i != _progressBars.end())
	{
		auto& b = i->second;
		b->width = width;
	}

	TraceDebug("Tried to set width of progressbar that was not a progressbar");
	return void();
}

const void EntityController::SetProgressBarMaxValue(const Entity & entity, const float value)
{
	auto i = _progressBars.find(entity);
	if (i != _progressBars.end())
	{
		auto& b = i->second;
		b->maxV = value;
	}

	TraceDebug("Tried to set max value of progressbar that was not a progressbar");
	return void();
}

const unsigned int EntityController::GetListSelectionValue(const Entity & entity) const
{
	auto i = _listSelections.find(entity);
	if (i != _listSelections.end())
		return i->second->value;

	TraceDebug("Tried to get value of selectionlist that was not a selectionlist");
	return 0;
}

const float EntityController::GetProgressBarValue(const Entity & entity) const
{
	auto i = _progressBars.find(entity);
	if (i != _progressBars.end())
		return i->second->value;

	TraceDebug("Tried to get value of progressbar that was not a progressbar");

	return 0.0f;
}

const float EntityController::GetSliderValue(const Entity & entity) const
{
	auto i = _sliders.find(entity);
	if (i != _sliders.end())
		return i->second->curr;

	TraceDebug("Tried to get value of slider that was not a slider");
	return 0.0f;
}

const Item* EntityController::GetScrollListItem(const Entity & entity, const uint & itemID) const
{
	auto i = _scrollLists.find(entity);
	if (i != _scrollLists.end())
		if (itemID < i->second->items.size())
		{
			return &i->second->items[itemID];
		}			
		else
		{
			TraceDebug("itemID %d out of range.", itemID);
			return nullptr;
		}
	TraceDebug("Tried to get value of slider that was not a slider");
	return nullptr;
}

const void EntityController::AddListSelection(const Entity & entity, ListSelection * listselection)
{
	auto i = _listSelections.find(entity);
	if (i != _listSelections.end())
		SAFE_DELETE(i->second);
	_listSelections[entity] = listselection;
}

const void EntityController::AddProgressBar(const Entity & entity, ProgressBar * progbar)
{
	auto i = _progressBars.find(entity);
	if (i != _progressBars.end())
		SAFE_DELETE(i->second);
	_progressBars[entity] = progbar;
}

const void EntityController::AddPopUpBox(const Entity & entity, PopUpBox * box)
{
	auto i = _popUps.find(entity);
	if (i != _popUps.end())
		SAFE_DELETE(i->second);
	_popUps[entity] = box;
}

const void EntityController::AddSlider(const Entity & entity, Slider * slider)
{
	auto i = _sliders.find(entity);
	if (i != _sliders.end())
		SAFE_DELETE(i->second);
	_sliders[entity] = slider;
}

const void EntityController::AddScrollList(const Entity & entity, ScrollList * list)
{
	auto i = _scrollLists.find(entity);
	if (i != _scrollLists.end())
		SAFE_DELETE(i->second);
	_scrollLists[entity] = list;
}

const void EntityController::ShowPopupBox(const Entity & entity)
{
	auto i = _popUps.find(entity);
	if (i != _popUps.end())
	{
		_popInfo.e = entity;
		_popInfo.poping = true;
	}
	TraceDebug("Tried to pop an entity that was not an popupbox.");
}


const void EntityController::Update() const
{
	_event->DoEvents();
	_animation->DoAnimations();
}

const void EntityController::SetExclusiveRenderAccess()const
{
	_overlay->BindToRenderer(true);
	_camera->BindToRenderer(true);
	_mesh->BindToRendered(true);
	_light->BindToRenderer(true);
	_text->BindToRenderer(true);
	_lightning->BindToRenderer(true);
}

const void EntityController::UnbindFromRenderer()const
{
	return void(); // TODO: Implement this
}

EntityManager& EntityController::EntityC()
{
	return _entity;
}
StaticMeshManager* EntityController::Mesh()const
{
	return _mesh;
}
TransformManager* EntityController::Transform()const
{
	return _transform;
}
CameraManager* EntityController::Camera()const
{
	return _camera;
}
MaterialManager* EntityController::Material()const
{
	return _material;
}
OverlayManager* EntityController::Overlay()const
{
	return _overlay;
}
EventManager* EntityController::Event()const
{
	return _event;
}
LightManager* EntityController::Light()const
{
	return _light;
}
BoundingManager* EntityController::Bounding()const
{
	return _bounding;
}
TextManager* EntityController::Text()const
{
	return _text;
}
LightningManager* EntityController::Lightning()const
{
	return _lightning;
}
DecalManager * EntityController::Decal() const
{
	return _decal;
}

AnimationManager * EntityController::Animation() const
{
	return _animation;
}
