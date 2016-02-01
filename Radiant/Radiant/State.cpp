#include "State.h"



State::State()
{
	_savedState = nullptr;
}


State::~State()
{
}

void State::Init()
{

}

void State::Shutdown()
{
	if (_savedState)
	{
		_savedState->Shutdown();
		delete _savedState;
		_savedState = nullptr;
	}
}

void State::HandleInput()
{
}

void State::Update()
{
}


const void State::DeleteManager()
{
	return void();
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
