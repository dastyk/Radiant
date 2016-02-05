#include "EntityController.h"
#include "System.h"

EntityController::EntityController(EntityManager & e, StaticMeshManager * mesh, TransformManager * trans, CameraManager * cam, MaterialManager * mat, OverlayManager * o, ClickingManager * click, LightManager * l, BoundingManager * b, TextManager * text) 
	: _entity(e), _mesh(mesh), _transform(trans), _camera(cam), _material(mat), _overlay(o), _clicking(click), _light(l), _bounding(b), _text(text)
{
}

EntityController::~EntityController()
{
}


const void EntityController::HandleInput() const
{
	auto i = System::GetInput();
	if(i->GetMouseKeyStateAndReset(VK_LBUTTON))
		_clicking->DoClick();

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
	return void();
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
ClickingManager* EntityController::Clicking()const
{
	return _clicking;
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