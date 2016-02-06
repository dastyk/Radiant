#ifndef _ENTITYCONTROLLER_H_
#define _ENTITYCONTROLLER_H_

#pragma once
#include "EntityManager.h"
#include "StaticMeshManager.h"
#include "TransformManager.h"
#include "CameraManager.h"
#include "OverlayManager.h"
#include "EventManager.h"
#include "LightManager.h"
#include "BoundingManager.h"
#include "TextManager.h"
class EntityController
{
public:
	EntityController(EntityManager& e, StaticMeshManager* mesh , TransformManager* trans, CameraManager* cam, MaterialManager* mat, OverlayManager* o, EventManager* _event, LightManager* l, BoundingManager* b, TextManager* text);
	~EntityController();


	const void BindEventHandler(const Entity& entity, const EventManager::Type& type);
	const void BindEvent(const Entity& entity, const EventManager::EventType& type, std::function<void()> callback);

	const void Update()const;

	const void SetExclusiveRenderAccess()const;
	const void UnbindFromRenderer()const;

	EntityManager& EntityC();
	StaticMeshManager* Mesh()const;
	TransformManager* Transform()const;
	CameraManager* Camera()const;
	MaterialManager* Material()const;
	OverlayManager* Overlay()const;
	EventManager* Event()const;
	LightManager* Light()const;
	BoundingManager* Bounding()const;
	TextManager* Text()const;

private:
	EntityManager _entity;
	StaticMeshManager* _mesh = nullptr;
	TransformManager* _transform = nullptr;
	CameraManager* _camera = nullptr;
	MaterialManager* _material = nullptr;
	OverlayManager* _overlay = nullptr;
	EventManager* _event = nullptr;
	LightManager* _light = nullptr;
	BoundingManager* _bounding = nullptr;
	TextManager* _text = nullptr;
};

#endif