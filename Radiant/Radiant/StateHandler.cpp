#include "StateHandler.h"
#include "System.h"


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
	if (_changeInfo.change)
	{
		_changeInfo.change = false;
		_currState->PauseTime();
		if (_changeInfo.passBuilder)
			_currState->PassBuilder(_changeInfo.state);
		else if(_changeInfo.noInit == false)
			_changeInfo.state->CreateBuilder();
		_changeInfo.state->SetExclusiveRenderAccess();
		if (_changeInfo.savePrevious)
			_changeInfo.state->SaveState(_currState);
		else
			Shutdown();

		_currState = _changeInfo.state;
		if (!_changeInfo.noInit)
			_currState->Init();
		else
			_currState->StartTime();
		
		return;
	}
	_currState->Update(); 
	_currState->Render();
}

const void StateHandler::ChangeState(StateChange & change)
{
	_changeInfo = std::move(change);
}
