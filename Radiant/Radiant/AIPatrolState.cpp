#include "AIPatrolState.h"
#include "Shodan.h"

AIPatrolState::AIPatrolState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder) : AIBaseState(currentState, controller, myEnemy, builder)
{
	_speedFactor = (rand() % 100 + 1) * 0.01f;
	_playerSeen = false;
	_myPath = nullptr;
	_timeUntilPlayerVanishes = 0.0f;
	_waiting = false;
	_waitTime = 0.0f;
	_timer = 0.0f;
	_movementVector = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_needTransition = false;
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
	Entity myEntity = _myEnemy->GetEntity();
	XMVECTOR currentPosition = _builder->Transform()->GetPosition(_myEnemy->GetEntity());
	int counter = 0;
	_myPath = nullptr;
	
	while(_myPath == nullptr)
	{
		_controller->EnemyStuck(_myEnemy->GetEntity());
		SAFE_DELETE(_myPath);
		_myPath = _controller->NeedPath(_myEnemy->GetEntity());
	}


	_nrOfStepsTaken = 0;

	_currentGoal = XMFLOAT3(_myPath->nodes[_nrOfStepsTaken].x + _myPath->nodes[_nrOfStepsTaken].offsetX, 0.5f, _myPath->nodes[_nrOfStepsTaken].y + _myPath->nodes[_nrOfStepsTaken].offsetY);

	XMStoreFloat3(&_movementVector, XMVector3Normalize(XMVectorSet(_currentGoal.x - XMVectorGetX(currentPosition), 0.0f, _currentGoal.z - XMVectorGetZ(currentPosition), 0.0f)));
	_nrOfStepsTaken++;
}

void AIPatrolState::Exit()
{
	SAFE_DELETE(_myPath);
}

void AIPatrolState::Update(float deltaTime)
{
	AIBaseState::Update(deltaTime);
	StatusEffects currentEffect = _myEnemy->GetCurrentStatusEffects();
	if ((currentEffect == STATUS_EFFECT_TIME_STOP) || (_myPath == nullptr))
	{
		if (_myPath == nullptr)
		{
			_controller->EnemyStuck(_myEnemy->GetEntity());
			_myPath = _controller->NeedPath(_myEnemy->GetEntity());
			return;
		}
		return;
	}

	_builder->Transform()->MoveAlongVector(_myEnemy->GetEntity(), XMLoadFloat3(&_movementVector)*deltaTime*_myEnemy->GetSpeedModification());
	XMVECTOR currentPosition = _builder->Transform()->GetPosition(_myEnemy->GetEntity());

	_timer += deltaTime;
	
	float length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&_currentGoal) - currentPosition));

	if (length < 0.03f)
	{
		_currentGoal = XMFLOAT3(_myPath->nodes[_nrOfStepsTaken].x + _myPath->nodes[_nrOfStepsTaken].offsetX, 0.5f, _myPath->nodes[_nrOfStepsTaken].y + _myPath->nodes[_nrOfStepsTaken].offsetY);
		_nrOfStepsTaken++;
	}

	if (_nrOfStepsTaken < _myPath->nrOfNodes)
	{
		XMStoreFloat3(&_movementVector, XMVector3Normalize(XMVectorSet(_currentGoal.x - XMVectorGetX(currentPosition), 0.0f, _currentGoal.z - XMVectorGetZ(currentPosition), 0.0f)));
	}
	else
	{
		SAFE_DELETE(_myPath);
		
		_myPath = _controller->NeedPath(_myEnemy->GetEntity());
		_nrOfStepsTaken = 0;
		_currentGoal = XMFLOAT3(_myPath->nodes[_nrOfStepsTaken].x + _myPath->nodes[_nrOfStepsTaken].offsetX, 0.5f, _myPath->nodes[_nrOfStepsTaken].y + _myPath->nodes[_nrOfStepsTaken].offsetY);
		
		XMStoreFloat3(&_movementVector, XMVector3Normalize(XMVectorSet(_currentGoal.x - XMVectorGetX(currentPosition), 0.0f, _currentGoal.z - XMVectorGetZ(currentPosition), 0.0f)));
		_nrOfStepsTaken++;
	}
	
}

void AIPatrolState::Init()
{

}

int AIPatrolState::CheckTransitions()
{
	//Check through all the "transitions" that can happen from this state. If we aren't going to get out, keep patrol.
	if (_controller->CheckIfPlayerIsSeenForEnemy(_myEnemy) || _myEnemy->GetCurrentStatusEffects() == STATUS_EFFECT_CHARMED)
	{
		return AI_STATE_ATTACK;
	}


	return AI_STATE_PATROL;
}

int AIPatrolState::GetType()
{
	return AI_STATE_PATROL;
}

void AIPatrolState::OnHit(float damage, StatusEffects effect, float duration)
{
	_myEnemy->ReduceHealth(damage);

	if (effect != STATUS_EFFECT_NORMAL)
		_myEnemy->SetStatusEffects(effect, duration);

	if (!_beenHit)
	{
		_beenHit = true;
		_resetIntensity = _builder->Light()->GetLightIntensity(_myEnemy->GetEntity());
		_glowOnHitTimer = 0.25f;
		_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), _resetIntensity*(250 * _glowOnHitTimer + 1));
	}
	else
	{
		_glowOnHitTimer += 0.05f;
	}
}

void AIPatrolState::GlobalStatus(StatusEffects effect, float duration)
{
	_myEnemy->SetStatusEffects(effect, duration);
}

void AIPatrolState::SetDamageModifier(float amount)
{
	_myEnemy->SetDamageMultiplier(amount);
}

void AIPatrolState::AddToDamageModifier(float amount)
{
	_myEnemy->AddToDamageMultiplier(amount);
}

void AIPatrolState::OnEnemyDeath()
{
	_myEnemy->AddToDamageMultiplier(damageModificationPerDeath);
	_myEnemy->AddToSpeedMofication(speedMoficationPerDeath);
}