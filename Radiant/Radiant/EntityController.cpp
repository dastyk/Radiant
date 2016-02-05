#include "EntityController.h"


EntityController::EntityController(EntityManager & e, StaticMeshManager * mesh, TransformManager * trans, CameraManager * cam, MaterialManager * mat, OverlayManager * o, ClickingManager * click, LightManager * l, BoundingManager * b, TextManager * text) 
	: _entity(e), _mesh(mesh), _transform(trans), _camera(cam), _material(mat), _overlay(o), _clicking(click), _light(l), _bounding(b), _text(text)
{
}

EntityController::~EntityController()
{
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