#ifndef _STATEHANDLER_H_
#define _STATEHANDLER_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "General.h"
#include "State.h"
#include "NullState.h"
#include "MenuState.h"
#include "GameState.h"
#include "TestState.h"
#include "OptionsState.h"
#include "GameOverState.h"

#define ChangeStateTo(x) {System::GetWindowHandler()->ChangeState(x);}

class StateHandler
{
public:
	StateHandler();
	~StateHandler();

public:
	void Init();
	void Shutdown();

	void Frame();
	const void ChangeState(StateChange& change);
private:
	State* _currState = nullptr;
	StateChange _changeInfo;
};

#endif