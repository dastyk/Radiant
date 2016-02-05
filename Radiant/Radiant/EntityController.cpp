#include "EntityController.h"



EntityController::EntityController()
{
}


EntityController::~EntityController()
{
}


const void EntityController::SetExclusiveRenderAccess()const
{
	overlay->BindToRenderer(true);
	camera->BindToRenderer(true);
	mesh->BindToRendered(true);
	light->BindToRenderer(true);
	text->BindToRenderer(true);
}

const void EntityController::UnbindFromRenderer()const
{
	return void();
}