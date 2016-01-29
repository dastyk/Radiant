#ifndef _MENUSTATE_H_
#define _MENUSTATE_H_

#include "State.h"

#include "Manager.h"

class MenuState :
	public State
{
public:
	MenuState();
	~MenuState();

	void Init();
	void Shutdown();

	void HandleInput();
	void Update();
	void Render();

private:
	EntityManager _entityManager;
	TransformManager* _transformManager = nullptr;
	StaticMeshManager* _staticMeshManager = nullptr;
	CameraManager* _cameraManager = nullptr;
	MaterialManager* _materialManager = nullptr;
	Entity _BTH;
	Entity _anotherOne;
	Entity _camera;

	ManagerWrapper _managers;
};

#endif