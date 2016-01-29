#ifndef _STATE_H_
#define _STATE_H_
#include "timer.h"

class State
{
protected:
	State();
	

public:
	virtual ~State();

	virtual void Init() = 0;
	virtual void Shutdown() = 0;

	virtual void HandleInput() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;


	virtual const void PassManager(void*);
	const void SaveState(State* pState);
	const State* GetSavedState();

protected:
	State* _savedState;
	Timer _gameTimer;
};

#endif
