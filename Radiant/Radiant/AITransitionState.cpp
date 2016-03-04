#include "AITransitionState.h"
#include "Shodan.h"

using namespace DirectX;


AITransitionState::AITransitionState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder) : AIBaseState(currentState, controller, myEnemy, builder), _nrOfStepsTaken(0)
{
	_waiting = false;
	_waitTime = 0.0f;
	_movementVector = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_reachedGoal = false;
}
AITransitionState::~AITransitionState()
{
	
}
void AITransitionState::Enter()
{
	float originalX, originalY;
	originalX =XMVectorGetX(_builder->Transform()->GetPosition(_myEnemy->GetEntity()));
	originalY = XMVectorGetZ(_builder->Transform()->GetPosition(_myEnemy->GetEntity()));

	if (_controller->NodeWalkable(originalX, originalY))
	{
		TraceDebug("Transition State worthless");
		//_builder->Transform()->SetPosition(_myEnemy->GetEntity(), XMVectorSet(originalX, 0.5f, originalY, 1.0f));
		_reachedGoal = true;
		return;
	}
	else
	{
		TraceDebug("Couldn't find a legit path for the enemy during Transistion");
	}
}
void AITransitionState::Exit()
{
	

}
void AITransitionState::Update(float deltaTime)
{
	AIBaseState::Update(deltaTime);
	if (_reachedGoal)
		return;
	_builder->Transform()->MoveAlongVector(_myEnemy->GetEntity(), XMLoadFloat3(&_movementVector)*deltaTime);
	float flooredPositionX, flooredPositionY;
	flooredPositionX = floor(XMVectorGetX(_builder->Transform()->GetPosition(_myEnemy->GetEntity())));
	flooredPositionY = floor(XMVectorGetZ(_builder->Transform()->GetPosition(_myEnemy->GetEntity())));
	if (_controller->NodeWalkable(flooredPositionX, flooredPositionY))
	{
		_builder->Transform()->SetPosition(_myEnemy->GetEntity(), XMVectorSet(flooredPositionX, 0.5f, flooredPositionY, 1.0f));
		_reachedGoal = true;
	}
}
void AITransitionState::Init()
{

}
int AITransitionState::CheckTransitions()
{
	if (_reachedGoal)
	{
		return AI_STATE_PATROL;
	}
	if (_controller->CheckIfPlayerIsSeenForEnemy(_myEnemy))
	{
		return AI_STATE_ATTACK;
	}
	return AI_STATE_TRANSITION;
}
int AITransitionState::GetType()
{
	return AI_STATE_TRANSITION;
}

void AITransitionState::OnHit(float damage, StatusEffects effect, float duration)
{
	_myEnemy->ReduceHealth(damage);
	_myEnemy->SetStatusEffects(effect, duration);
}

void AITransitionState::GlobalStatus(StatusEffects effect, float duration)
{
	_myEnemy->SetStatusEffects(effect, duration);
}

void AITransitionState::SetDamageModifier(float amount)
{
	_myEnemy->SetDamageMultiplier(amount);
}

void AITransitionState::AddToDamageModifier(float amount)
{
	_myEnemy->AddToDamageMultiplier(amount);
}

void AITransitionState::OnEnemyDeath()
{
	_myEnemy->AddToDamageMultiplier(damageModificationPerDeath);
	_myEnemy->AddToSpeedMofication(speedMoficationPerDeath);
}