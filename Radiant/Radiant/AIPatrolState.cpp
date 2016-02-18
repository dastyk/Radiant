#include "AIPatrolState.h"
#include "Shodan.h"

AIPatrolState::AIPatrolState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder) : AIBaseState(currentState, controller, myEnemy, builder)
{
	_speedFactor = (rand() % 100 + 1) * 0.97f;
	_playerSeen = false;
	_myPath = nullptr;
	_timeUntilPlayerVanishes = 0.0f;
	_waiting = false;
	_waitTime = 0.0f;
	_timer = 0.0f;
	_movementVector = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

AIPatrolState::~AIPatrolState()
{
	if (_myPath)
	{
		delete[] _myPath->nodes;
		_myPath->nodes = nullptr;
		delete _myPath;
		_myPath = nullptr;
	}
}

void AIPatrolState::Enter()
{
	_myPath = _controller->NeedPath(_myEnemy->GetEntity());
	if (!_myPath)
	{
		TraceDebug("No path returned to the Patrol State");
		return;
	}
	XMVECTOR move = XMLoadFloat3(&_movementVector);
	XMVECTOR temp = _builder->Transform()->GetPosition(_myEnemy->GetEntity());
	if (_myPath->nrOfNodes)
	{
		_nrOfStepsTaken = 0;
		_currentGoal = XMFLOAT3(_myPath->nodes[_nrOfStepsTaken].x - 0.5f, 0, _myPath->nodes[_nrOfStepsTaken].y - 0.5f);
		move = XMVectorSetX(move, _currentGoal.x - XMVectorGetX(temp));
		move = XMVectorSetZ(move, _currentGoal.z - XMVectorGetZ(temp));
		move = XMVector3Normalize(move);
		XMStoreFloat3(&_movementVector, move);
		_waitTime = rand() % 10000;
		_waiting = true;
	}
	else
	{
		if (_myPath)
		{
			delete[] _myPath->nodes;
			_myPath->nodes = nullptr;
			delete _myPath;
			_myPath = nullptr;
		}
		TraceDebug("No real path returned when entering Patrol State. Retrying.");
		Enter();
	}

}

void AIPatrolState::Exit()
{
	if (_myPath)
	{
		delete[] _myPath->nodes;
		_myPath->nodes = nullptr;
		delete _myPath;
		_myPath = nullptr;
	}
}

void AIPatrolState::Update(float deltaTime)
{
	Entity myEntity = _myEnemy->GetEntity();
	XMVECTOR move = XMLoadFloat3(&_movementVector);
	if (_waiting)
	{
		_waitTime -= deltaTime;
		if (_waitTime < 0.0f)
		{
			_waiting = false;
		}
	}
	if (!_myPath)
	{
		TraceDebug("Things heavily fucked up.");
		return;
	}
	_timer += deltaTime;
	if (_timer > 0.02f)
	{
		if(_controller->PlayerSeen())
		{
			if (!_playerSeen)
			{
				XMVECTOR temp = _builder->Transform()->GetPosition(myEntity);
				_builder->Transform()->SetPosition(myEntity, XMVectorSet(_currentGoal.x + 0.5f, 0, _currentGoal.z + 0.5f, 1.0f));
				if (_myPath)
				{
					delete[] _myPath->nodes;
					_myPath->nodes = nullptr;
					delete _myPath;
					_myPath = nullptr;
				}
				_myPath = _controller->NeedPath(myEntity);	
				if (!_myPath)
				{
					TraceDebug("No path found towards the player");
					return;
				}
				XMVECTOR move = XMLoadFloat3(&_movementVector);
				_builder->Transform()->SetPosition(myEntity, temp);
				_playerSeen = true; 
				if (_myPath->nrOfNodes)
				{
					_nrOfStepsTaken = 0;
					_currentGoal = XMFLOAT3(_myPath->nodes[_nrOfStepsTaken].x - 0.5f, 0, _myPath->nodes[_nrOfStepsTaken].y - 0.5f);
					move = XMVectorSetX(move, _currentGoal.x - XMVectorGetX(temp));
					move = XMVectorSetZ(move, _currentGoal.z - XMVectorGetZ(temp));
					move = XMVector3Normalize(move);
					XMStoreFloat3(&_movementVector, move);
					_waitTime = rand() % 10000;
					_waiting = true;
				}
			}
			_timeUntilPlayerVanishes = 1.25f;
		}
		else
		{
			if (_playerSeen)
			{
				_timeUntilPlayerVanishes -= deltaTime;
				if (_timeUntilPlayerVanishes < 0.0f)
				{
					_playerSeen = false;
				}
			}
		}
		XMVECTOR test = _builder->Transform()->GetPosition(myEntity);
		move = XMVectorSetX(move, _currentGoal.x - XMVectorGetX(test));
		move = XMVectorSetZ(move, _currentGoal.z - XMVectorGetZ(test));
		move = XMVector3Normalize(move);
		XMStoreFloat3(&_movementVector, move);
		_timer = 0.00f;
	}
	if (_nrOfStepsTaken > _myPath->nrOfNodes)
	{

		XMVECTOR temp = _builder->Transform()->GetPosition(myEntity);
		_builder->Transform()->SetPosition(myEntity, XMVectorSet(_currentGoal.x + 0.5f, 0, _currentGoal.z + 0.5f, 1.0f));
		if (_myPath)
		{
			delete[] _myPath->nodes;
			_myPath->nodes = nullptr;
			delete _myPath;
			_myPath = nullptr;
		}
		_myPath = _controller->NeedPath(myEntity);
		if (!_myPath)
		{
			TraceDebug("No path returned to the Patrol State");
			return;
		}
		XMVECTOR move = XMLoadFloat3(&_movementVector);
		_builder->Transform()->SetPosition(myEntity, temp);
		if (_myPath->nrOfNodes)
		{
			_nrOfStepsTaken = 0;
			_currentGoal = XMFLOAT3(_myPath->nodes[_nrOfStepsTaken].x - 0.5f, 0, _myPath->nodes[_nrOfStepsTaken].y - 0.5f);
			move = XMVectorSetX(move, _currentGoal.x - XMVectorGetX(temp));
			move = XMVectorSetZ(move, _currentGoal.z - XMVectorGetZ(temp));
			move = XMVector3Normalize(move);
			XMStoreFloat3(&_movementVector, move);
			_waitTime = rand() % 10000;
			_waiting = true;
		}
	}
	_builder->Transform()->MoveAlongVector(myEntity, move*deltaTime);
	XMVECTOR test = XMVectorSetY(_builder->Transform()->GetPosition(myEntity), 0.0f);
	float testValue = XMVectorGetX(XMVector3Length(test - XMLoadFloat3(&_currentGoal)));
	if (testValue < 0.01)
	{
		int didIChange = rand() % 1000;
		if (didIChange == 0)
		{
			testValue = 0.0f;
		}
	}
	if (testValue < 0.007f)
	{
		XMVECTOR test = _builder->Transform()->GetPosition(myEntity);
		if (_nrOfStepsTaken != _myPath->nrOfNodes)
		{
			_nrOfStepsTaken++;
			_currentGoal = XMFLOAT3(_myPath->nodes[_nrOfStepsTaken].x - 0.5f, 0, _myPath->nodes[_nrOfStepsTaken].y - 0.5f);
			move = XMVectorSetX(move, _currentGoal.x - XMVectorGetX(test));
			move = XMVectorSetY(move, 0.0f);
			move = XMVectorSetZ(move, _currentGoal.z - XMVectorGetZ(test));
			move = XMVector3Normalize(move);
			XMStoreFloat3(&_movementVector, move);
		}
		else
		{
			_nrOfStepsTaken++;
		}

	}

}

void AIPatrolState::Init()
{

}

int AIPatrolState::CheckTransitions()
{
	//Check through all the "transitions" that can happen from this state. If we aren't going to get out, keep patrol.



	return AI_STATE_PATROL;
}

int AIPatrolState::GetType()
{
	return AI_STATE_PATROL;
}