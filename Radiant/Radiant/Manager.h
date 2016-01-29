#ifndef _MANAGER_H_
#define _MANAGER_H_


#pragma once
#include "EntityManager.h"
#include "StaticMeshManager.h"
#include "TransformManager.h"
#include "CameraManager.h"
#include "Utils.h"

struct ManagerWrapper
{
	EntityManager entity;
	StaticMeshManager* mesh;
	TransformManager* transform;
	CameraManager* camera;

	ManagerWrapper()
	{
		transform = new TransformManager();
		mesh = new StaticMeshManager(*transform);
		camera = new CameraManager(*transform);
	}
	~ManagerWrapper()
	{
		SAFE_DELETE(camera);
		SAFE_DELETE(mesh);
		SAFE_DELETE(transform);
	}
};



#endif
