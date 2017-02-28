#ifndef  APL_STATE_H_

#pragma once
#include "State.h"

class APLState :
	public State
{
public:
	APLState();
	~APLState();

	virtual void Init();
	virtual void Shutdown();

	//virtual void Update() ;
	//virtual void Render() ;
private:
	const void _BuildScene();
	const void _SetupInput();
private:
	Entity _camera;
	void* _dungeon;
};


#endif // ! APL_STATE_H_

