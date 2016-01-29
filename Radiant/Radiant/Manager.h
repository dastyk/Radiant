#ifndef _MANAGER_H_
#define _MANAGER_H_


#pragma once
#include "EntityManager.h"
#include "StaticMeshManager.h"
#include "TransformManager.h"
#include "CameraManager.h"
#include "Utils.h"
#include "OverlayManager.h"

struct ManagerWrapper
{
	EntityManager entity;
	StaticMeshManager* mesh;
	TransformManager* transform;
	CameraManager* camera;
	MaterialManager* material;
	OverlayManager* overlay;

	ManagerWrapper()
	{
		transform = new TransformManager();
		material = new MaterialManager();
		mesh = new StaticMeshManager(*transform, *material);
		overlay = new OverlayManager(*transform, *material);
		camera = new CameraManager(*transform);
	}
	~ManagerWrapper()
	{
		SAFE_DELETE(camera);
		SAFE_DELETE(mesh);
		SAFE_DELETE(overlay);
		SAFE_DELETE(transform);
		SAFE_DELETE(material);
	}

	const void SetExclusiveRenderAccess()
	{
		auto g = System::GetGraphics();
		g->ClearCameraProviders();
		g->ClearOverlayProviders();
		g->ClearRenderProviders();

		overlay->BindToRenderer();
		camera->BindToRenderer();
		mesh->BindToRendered();
	}
};



#endif
