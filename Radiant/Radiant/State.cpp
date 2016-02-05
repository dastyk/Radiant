#include "State.h"
#include "Utils.h"

State::State()
{
	_builder = nullptr;
	try { _builder = new EntityBuilder; }
	catch (std::exception& e) { e; throw ErrorMsg(3000002, L"Failed to create EntityBuilder in state."); }
	_controller = _builder->GetEntityController();
	_passed = false;

	_builder->GetEntityController()->SetExclusiveRenderAccess();
	_savedState = nullptr;

	_gameTimer.Start();
	_gameTimer.Reset();
}


State::~State()
{
	if (_savedState)
	{
		_savedState->Shutdown();
		delete _savedState;
		_savedState = nullptr;
	}
	if (!_passed)
		SAFE_DELETE(_builder);
}

void State::Init()
{

}

void State::Shutdown()
{

}

void State::HandleInput()
{
}

void State::Update()
{
}

const void State::SaveState(State * pState)
{
	_savedState = pState;
	return void();
}

const State* State::GetSavedState()
{
	return _savedState;
}

const void State::SaveBuilder(EntityBuilder* builder)
{
	if (_builder)
	{
		throw ErrorMsg(3000004, L"Tried to pass builder to state that already had one.");
	}
	_passed = false;
	_builder = builder;
	_controller = _builder->GetEntityController();
}

const void State::PassBuilder(State* state)
{
	try {
		state->SaveBuilder(_builder);
	}
	catch (ErrorMsg&msg)
	{
		TraceDebug("Tried to pass manager to a state that already had one.");
		return;
	}
	_passed = true;
	_builder = nullptr;
}

const EntityBuilder* State::GetBuilder()
{
	if (!_builder)
	{
		TraceDebug("Tried to get manager from state that had none, created a new one instead.");
		return new EntityBuilder();
	}
	auto temp = _builder;
	_builder = nullptr;
	return temp;
}

const void State::PauseTime()
{
	_gameTimer.Stop();
}

const void State::StartTime()
{
	_gameTimer.Start();
}


