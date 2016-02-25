#include "AITeleportMoveState.h"
#include "Shodan.h"

AITeleportMoveState::AITeleportMoveState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder) : AIBaseState(currentState, controller, myEnemy, builder)
{
	_myPath = nullptr;
	_waitTime = (rand() % 3 + 1)*1.2f + 1.5f;
	_timer = 0.0f;
	_originalIntensity = _builder->Light()->GetLightIntensity(_myEnemy->GetEntity());
	_nrOfStepsTaken = 0.0f;
	_arrived = true;
}

AITeleportMoveState::~AITeleportMoveState()
{
	SAFE_DELETE(_myPath);
}

void AITeleportMoveState::Enter()
{
	Entity myEntity = _myEnemy->GetEntity();
	_myPath = _controller->NeedPath(myEntity);
	while (_myPath->nrOfNodes < 11)
	{
		SAFE_DELETE(_myPath);
		_myPath = _controller->NeedPath(myEntity);
	}
	_nrOfStepsTaken = 0;
	_currentGoal = XMFLOAT3(_myPath->nodes[_nrOfStepsTaken + 10].x + _myPath->nodes[_nrOfStepsTaken + 10].offsetX, 0.5f, _myPath->nodes[_nrOfStepsTaken + 10].y + _myPath->nodes[_nrOfStepsTaken + 10].offsetY);
	_myEnemy->SetCurrentWeapon(new EnemyDeathBlossomWeapon(_builder, _myEnemy->GetColor()));
}

void AITeleportMoveState::Exit()
{
	SAFE_DELETE(_myPath);
}

void AITeleportMoveState::Update(float deltaTime)
{
	AIBaseState::Update(deltaTime);
	
	if (_arrived)
	{
		_timer += deltaTime;
		if (_timer > _waitTime)
		{
			_myEnemy->GetWeapon()->Shoot();
			Entity a = _myEnemy->GetEntity();
			XMVECTOR currentPos = _builder->GetEntityController()->Transform()->GetPosition(a);
			_builder->Transform()->SetPosition(_myEnemy->GetEntity(), _currentGoal);
			while (_myPath->nrOfNodes < _nrOfStepsTaken + 10)
			{
				SAFE_DELETE(_myPath);
				_myPath = _controller->NeedPath(_myEnemy->GetEntity());
				_nrOfStepsTaken = 0;
			}
			_currentGoal = XMFLOAT3(_myPath->nodes[_nrOfStepsTaken + 10].x + _myPath->nodes[_nrOfStepsTaken + 10].offsetX, 0.5f, _myPath->nodes[_nrOfStepsTaken + 10].y + _myPath->nodes[_nrOfStepsTaken + 10].offsetY);
			_nrOfStepsTaken += 5;

			_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), _originalIntensity);
			_arrived = false;
		}
		else
		{
			_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), min(_originalIntensity, _originalIntensity*(_waitTime - _timer + 0.2f)));
		}
	}
	else
	{
		_timer -= 2*deltaTime;
		if (_timer < 0.0f)
		{
			_timer = 0.0f;
			_arrived = true;
		}
		_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), min(_originalIntensity, _originalIntensity*(_waitTime - _timer + 0.2f)));
	}
}

void AITeleportMoveState::Init()
{

}

int AITeleportMoveState::CheckTransitions()
{
	//The teleport enemy cannot attack, but will only teleport around and be cool
	return AI_STATE_PATROL;
}

int AITeleportMoveState::GetType()
{
	return AI_STATE_PATROL;
}