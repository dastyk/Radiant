#ifndef _STATE_H_
#define _STATE_H_
#include "timer.h"
#include "EntityBuilder.h"

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

	virtual const void SaveState(State* pState);
	virtual const State* GetSavedState();
	virtual const void State::SaveBuilder(EntityBuilder* builder);
	virtual const void State::PassBuilder(State* state);
	virtual const EntityBuilder* State::GetBuilder();
	virtual const void PauseTime();
	virtual const void StartTime();
protected:
	State* _savedState;
	Timer _gameTimer;
	EntityBuilder* _builder;
	EntityController* _controller;
	bool _passed;
};

struct StateChange
{
	State* state;
	bool savePrevious;
	bool noInit;
	bool passBuilder;
	StateChange(State* pS, bool savePrev = false, bool noInit = false, bool passBuilder = false) : state(pS), savePrevious(savePrev), noInit(noInit), passBuilder(passBuilder)
	{ };
};

#endif
