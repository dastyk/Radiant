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
	Entity _camera;
};


#endif // ! APL_STATE_H_

