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

void State::ShutDown()
{
	if (_savedState)
	{
		_savedState->ShutDown();
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

void State::Render()
{
}


State* State::GetSavedState() const
{
	return _savedState;
}
