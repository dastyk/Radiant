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
	StaticMeshManager* mesh = nullptr;
	TransformManager* transform = nullptr;
	CameraManager* camera = nullptr;
	MaterialManager* material = nullptr;
	OverlayManager* overlay = nullptr;

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
		overlay->BindToRenderer(true);
		camera->BindToRenderer(true);
		mesh->BindToRendered(true);
	}

	Entity& CreateCamera(XMVECTOR& position)const;
	Entity& CreateObject(XMVECTOR & pos, XMVECTOR & rot, XMVECTOR & scale, std::string& meshtext, std::string& texture, std::string& normal)const;
	//Entity& CreateInvisibleObject(XMVECTOR& pos, XMVECTOR& rot, XMVECTOR& scale)const;

};



#endif
