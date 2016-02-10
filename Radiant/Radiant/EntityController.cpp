#include "EntityController.h"
#include "System.h"

EntityController::EntityController(EntityManager & e, StaticMeshManager * mesh, TransformManager * trans, CameraManager * cam, MaterialManager * mat, OverlayManager * o, EventManager * _event, LightManager * l, BoundingManager * b, TextManager * text)
	: _entity(e), _mesh(mesh), _transform(trans), _camera(cam), _material(mat), _overlay(o), _event(_event), _light(l), _bounding(b), _text(text)
{
}

EntityController::~EntityController()
{
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

const void EntityController::Update() const
{
	_event->DoEvents();

}

const void EntityController::SetExclusiveRenderAccess()const
{
	_overlay->BindToRenderer(true);
	_camera->BindToRenderer(true);
	_mesh->BindToRendered(true);
	_light->BindToRenderer(true);
	_text->BindToRenderer(true);
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