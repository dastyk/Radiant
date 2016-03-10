#ifndef _PAUSESTATE_H_
#define _PAUSESTATE_H_

#pragma once
#include "State.h"

class PauseState : public State
{
public:
	PauseState();
	~PauseState();

	virtual void Init();
	virtual void Shutdown();
};

#endif