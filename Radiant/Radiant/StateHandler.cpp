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

}

void StateHandler::ShutDown()
{
	if (_currState)
	{
		_currState->ShutDown();
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
			ShutDown();
		_currState = rSC.state;
		_currState->Init();
	}

	_currState->Update();
	_currState->Render();
}
