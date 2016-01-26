#ifndef _NULLSTATE_H_
#define _NULLSTATE_H_

#pragma once
#include "State.h"
class NullState :
	public State
{
public:
	NullState();
	~NullState();

	void Init();
	void Shutdown();

	void HandleInput();
	void Update();
	void Render();
};

#endif