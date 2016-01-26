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

class StateHandler
{
public:
	StateHandler();
	~StateHandler();

public:
	void Init();
	void ShutDown();

	void Frame();

private:
	State* _currState;
};

#endif