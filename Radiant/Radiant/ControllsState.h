#ifndef _CONTROLLSSTATE_H_
#define _CONTROLLSSTATE_H_

#pragma once
#include "State.h"
class ControllsState :
	public State
{
public:
	ControllsState();
	~ControllsState();



	void Init();
	void Shutdown();

};

#endif