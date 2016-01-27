#include "StateHandler.h"



StateHandler::StateHandler()
{
	_currState = nullptr;
}


StateHandler::~StateHandler()
{
}

void StateHandler::Init()
{
	try{ _currState = new MenuState; }
	catch (std::exception& e)
	{
		e;
		throw ErrorMsg(3000001, L"Failed to create MenuState");
	}

	_currState->Init();
}

void StateHandler::Shutdown()
{
	if (_currState)
	{
		_currState->Shutdown();
		delete _currState;
		_currState = nullptr;
	}
}

void StateHandler::Frame()
{
	try { _currState->HandleInput(); }
	catch (StateChange& rSC)
	{
		if (rSC.savePrevious)
			rSC.state->SaveState(_currState); 
		else
			Shutdown();
		_currState = rSC.state;
		_currState->Init();
	}

	_currState->Update();
	_currState->Render();
}
