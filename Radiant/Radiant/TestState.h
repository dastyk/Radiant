#ifndef _TESTSTATE_H_
#define _TESTSTATE_H_

#pragma once
#include "State.h"
#include "General.h"
#include "CPUTimer.h"

class TestState :
	public State
{
public:
	TestState();
	~TestState();


	void Init();
	void Shutdown();

	void HandleInput();
	void Update();
	void Render();

private:
	Entity _BTH;
	Entity _point;
	Entity _anotherOne;
	Entity _camera;
	Entity _overlay;
	Entity _overlaytest2;
	Entity test2;
	Entity test;
	CPUTimer _timer;
};

#endif