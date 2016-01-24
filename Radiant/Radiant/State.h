#ifndef _STATE_H_
#define _STATE_H_

#pragma once
class State
{
protected:
	State();
	

public:
	virtual ~State();

	virtual void Init() = 0;
	virtual void ShutDown() = 0;

	virtual void HandleInput() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;

	const void SaveState(State* pState);
	const State* GetSavedState();

private:
	State* _savedState;
};

#endif
