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
	_currState->CreateBuilder();
	//try { _currState = new GameState; }
	//catch (std::exception& e)
	//{
	//	e;
	//	throw ErrorMsg(3000003, L"Failed to create GameState");
	//}

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
	try { _currState->Update(); }
	catch (StateChange& rSC)
	{
		_currState->PauseTime();
		if (rSC.passBuilder)
			_currState->PassBuilder(rSC.state);
		else
			rSC.state->CreateBuilder();
		if (rSC.savePrevious)
			rSC.state->SaveState(_currState); 
		else
			Shutdown();		
		
		_currState = rSC.state;
		if (!rSC.noInit)
			_currState->Init();
		else
			_currState->StartTime();

		return;
	}
	_currState->Render();
}
