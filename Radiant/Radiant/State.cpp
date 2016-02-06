#include "State.h"
#include "Utils.h"
#include "System.h"
State::State()
{
	_savedState = nullptr;

	_gameTimer.Start();
	_gameTimer.Reset();
}


State::~State()
{
	SAFE_SHUTDOWN(_savedState);
	if (!_passed)
		SAFE_DELETE(_builder);
}

void State::Init()
{
	_builder->GetEntityController()->SetExclusiveRenderAccess();
}

void State::Shutdown()
{

}

void State::Update()
{
	_controller->Update();
}

void State::Render()
{
	System::GetGraphics()->Render(_gameTimer.TotalTime(), _gameTimer.DeltaTime());
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

const void State::CreateBuilder()
{
	_builder = nullptr;
	try { _builder = new EntityBuilder; }
	catch (std::exception& e) { e; throw ErrorMsg(3000002, L"Failed to create EntityBuilder in state."); }
	_controller = _builder->GetEntityController();
	_controller->SetExclusiveRenderAccess();
	_passed = false;

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


