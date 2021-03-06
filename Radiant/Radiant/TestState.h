#ifndef _TESTSTATE_H_
#define _TESTSTATE_H_

#pragma once
#include "State.h"
#include "General.h"
#include "CPUTimer.h"
#include "Dungeon.h"
#include "Player.h"
#include "Shodan.h"

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

	Entity _map;
	Entity _camera;
	Entity _BTHLogo;
	Entity _BTHLogo2;
	Entity _lightningBase;
	Entity _lightningTarget;
	Timer _lightningTimer;
	Player* _player;
	CPUTimer _timer;
	Shodan* _AI;
	Dungeon* _dungeon;
	float _lightLevel;
};

#endif