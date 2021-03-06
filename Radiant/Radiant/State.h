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

	virtual void Update();
	virtual void Render();

	virtual const void CreateBuilder();
	virtual const EntityBuilder* State::GetBuilder();

	virtual const void SaveState(State* pState);
	virtual const State* GetSavedState();
	virtual const void State::SaveBuilder(EntityBuilder* builder);
	virtual const void State::PassBuilder(State* state);

	virtual const void State::SetExclusiveRenderAccess();

	virtual const void PauseTime();
	virtual const void StartTime();
protected:
	State* _savedState;
	Timer _gameTimer;
	EntityBuilder* _builder;
	EntityController* _controller;
	bool _passed = false;
};

struct StateChange
{
	State* state;
	bool savePrevious;
	bool noInit;
	bool passBuilder;
	bool change;
	StateChange():state(nullptr), savePrevious(false), noInit(false), passBuilder(false), change(false) {}
	StateChange(State* pS, bool savePrev = false, bool noInit = false, bool passBuilder = false) : state(pS), savePrevious(savePrev), noInit(noInit), passBuilder(passBuilder), change(true)
	{ };
};

#endif
