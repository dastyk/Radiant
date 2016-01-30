#ifndef _MENUSTATE_H_
#define _MENUSTATE_H_

#include "State.h"

#include "Manager.h"

class MenuState :
	public State
{
public:
	MenuState();
	MenuState(ManagerWrapper* wrapper);
	~MenuState();

	void Init();
	void Shutdown();

	void HandleInput();
	void Update();
	void Render();

	const void DeleteManager();
private:
	Entity _BTH;
	Entity _anotherOne;
	Entity _camera;
	Entity _overlay;
	ManagerWrapper* _managers;
	bool _passed;
};

#endif