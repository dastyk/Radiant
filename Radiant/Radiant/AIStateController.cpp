#include "AIStateController.h"
#include <algorithm>


AIStateController::AIStateController(int type)
{
	_type = type;
	_allowedToChangeState = true;
	_thinkingTime = 1.0f;
	_timeSinceTransistion = 0.0f;
	_currentState = nullptr;
	_defaultState = nullptr;
}

AIStateController::~AIStateController()
{
	while(_AIStates.size())
	{
		_AIStates.back()->Exit();
		delete _AIStates.back();
		_AIStates.pop_back();
	}

}

void AIStateController::UpdateMachine(float deltaTime)
{
	//Return if: We have no states in the states vector or there aren't any current state or default state.
	if (_AIStates.size() == 0)
	{
		TraceDebug("No states exists for the enemy.");
		return;
	}
	if (!_currentState)
	{
		TraceDebug("No CurrentState for the enemy");
		_currentState = _defaultState;
		if (!_currentState)
		{
			TraceDebug("No DefaultState for the enemy");
			return;
		}
	}

	if (!_allowedToChangeState)
	{
		_timeSinceTransistion += deltaTime;
		if (_timeSinceTransistion > _thinkingTime)
		{
			_allowedToChangeState = true;
		}
	}

	if (_allowedToChangeState)
	{
		_goalID = _currentState->CheckTransitions();
		if (_goalID != _currentState->GetType())
		{
			if (TransitionState(_goalID))
			{
				_currentState->Exit();
				_currentState = _goalState;
				_currentState->Enter();
				_allowedToChangeState = false;
				_timeSinceTransistion = 0.0f;
			}
		}

	}


	_currentState->Update(deltaTime);
}

void AIStateController::AddState(AIBaseState* state)
{
	for (auto &it : _AIStates)
	{
		if (it->GetType() == state->GetType())
		{
			TraceDebug("That state has already been added to the StateController");
			delete state;
			return;
		}
	}
	if (_AIStates.size() == 0)
	{
		_defaultState = state;
		_defaultState->Enter();
	}
	_AIStates.push_back(state);
	
}

void AIStateController::SetDefaultState(AIBaseState* state)
{ 
	_defaultState = state; 
}

void AIStateController::SetGoalID(int goal)
{ 
	_goalID = goal; 
}

bool AIStateController::TransitionState(int goal)
{
	for (auto &it : _AIStates)
	{
		if (it->GetType() == goal)
		{
			return true;
		}
	}
	return false;
}

void AIStateController::Reset()
{
	//Reset everything to default state, with the default state as current. All states are there in the vector, all the time.
	if (!_defaultState)
	{
		return;
	}
	_type = _defaultState->GetType();
	_currentState = _defaultState;
	_allowedToChangeState = true;
	_thinkingTime = 1.0f;
	_timeSinceTransistion = 0.0f;
}

void AIStateController::SetThinkingTime(float time)
{
	_thinkingTime = time;
}