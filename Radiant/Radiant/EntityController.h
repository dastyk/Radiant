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
	EntityController();
	~EntityController();

	const void SetExclusiveRenderAccess()const;
	const void UnbindFromRenderer()const;
private:
	EntityManager entity;
	StaticMeshManager* mesh = nullptr;
	TransformManager* transform = nullptr;
	CameraManager* camera = nullptr;
	MaterialManager* material = nullptr;
	OverlayManager* overlay = nullptr;
	ClickingManager* clicking = nullptr;
	LightManager* light = nullptr;
	BoundingManager* bounding = nullptr;
	TextManager* text = nullptr;
};

#endif