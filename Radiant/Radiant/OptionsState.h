#pragma once
#ifndef _OPTIONS_STATE_H_
#define _OPTIONS_STATE_H_

#include "State.h"

class OptionsState : public State
{
public:
	OptionsState();
	~OptionsState();

	void Init();
	void Shutdown();
private:
	const bool _HasChanges()const { return (_changes>0)? true : false; };
private:
	uint _changes = 0;
};

#endif