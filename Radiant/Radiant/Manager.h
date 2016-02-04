#ifndef _MANAGER_H_
#define _MANAGER_H_


#pragma once
#include "EntityManager.h"
#include "StaticMeshManager.h"
#include "TransformManager.h"
#include "CameraManager.h"
#include "Utils.h"
#include "OverlayManager.h"
#include "ClickingManager.h"
#include "LightManager.h"
#include "BoundingManager.h"
#include "TextManager.h"

struct ManagerWrapper
{
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
	ManagerWrapper()
	{
		transform = new TransformManager();
		material = new MaterialManager();
		mesh = new StaticMeshManager(*transform, *material);
		overlay = new OverlayManager(*transform, *material);
		camera = new CameraManager(*transform);
		clicking = new ClickingManager(*transform, *overlay);
		light = new LightManager(*transform);
		bounding = new BoundingManager(*transform);
		text = new TextManager(*transform);
	}
	~ManagerWrapper()
	{
		SAFE_DELETE(text);
		SAFE_DELETE(bounding);
		SAFE_DELETE(clicking);
		SAFE_DELETE(camera);
		SAFE_DELETE(mesh);
		SAFE_DELETE(overlay);
		SAFE_DELETE(transform);
		SAFE_DELETE(material);
		SAFE_DELETE(light);
	}

	const void SetExclusiveRenderAccess();

	Entity CreateCamera(XMVECTOR& position);
	Entity CreateObject(XMVECTOR & pos, XMVECTOR & rot, XMVECTOR & scale, std::string meshtext, std::string texture, std::string normal);
	Entity CreateOverlay(XMVECTOR & pos, float width, float height, std::string texture);
	//Entity& CreateInvisibleObject(XMVECTOR& pos, XMVECTOR& rot, XMVECTOR& scale)const;

};



#endif
