#ifndef _MENUSTATE_H_
#define _MENUSTATE_H_

#include "State.h"

#include "EntityManager.h"
#include "StaticMeshManager.h"

//#include "Audio.h" // temp for test
class Audio;

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
	StaticMeshManager* _staticMeshManager = nullptr;

	Entity _BTH;
	Audio* testAudio; // temp for test
};

#endif