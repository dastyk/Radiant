#ifndef _MENUSTATE_H_
#define _MENUSTATE_H_

#include "State.h"

#include "EntityManager.h"
#include "TransformManager.h"
#include "StaticMeshManager.h"

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

	Entity _BTH;
	Entity _anotherOne;
};

#endif