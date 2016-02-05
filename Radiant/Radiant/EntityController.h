#ifndef _ENTITYCONTROLLER_H_
#define _ENTITYCONTROLLER_H_

#pragma once
#include "EntityManager.h"
#include "StaticMeshManager.h"
#include "TransformManager.h"
#include "CameraManager.h"
#include "OverlayManager.h"
#include "ClickingManager.h"
#include "LightManager.h"
#include "BoundingManager.h"
#include "TextManager.h"
class EntityController
{
public:
	EntityController(EntityManager& e, StaticMeshManager* mesh , TransformManager* trans, CameraManager* cam, MaterialManager* mat, OverlayManager* o, ClickingManager* click, LightManager* l, BoundingManager* b, TextManager* text);
	~EntityController();

	const void SetExclusiveRenderAccess()const;
	const void UnbindFromRenderer()const;
private:
	EntityManager _entity;
	StaticMeshManager* _mesh = nullptr;
	TransformManager* _transform = nullptr;
	CameraManager* _camera = nullptr;
	MaterialManager* _material = nullptr;
	OverlayManager* _overlay = nullptr;
	ClickingManager* _clicking = nullptr;
	LightManager* _light = nullptr;
	BoundingManager* _bounding = nullptr;
	TextManager* _text = nullptr;
};

#endif