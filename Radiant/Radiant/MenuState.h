#ifndef _MENUSTATE_H_
#define _MENUSTATE_H_

#pragma once
#include "State.h"
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
};

#endif