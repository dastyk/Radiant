#include "AIAttackState.h"
#include "Shodan.h"
#include "EnemyBasicWeapon.h"

AIAttackState::AIAttackState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder) : AIBaseState(currentState, controller, myEnemy, builder)
{
	_myEnemy->SetCurrentWeapon(new EnemyBasicWeapon(_builder, myEnemy->GetColor()));
	_lastAttackVector = XMFLOAT3(0.0f, 0.0f, 0.0f);
}
AIAttackState::~AIAttackState()
{

}
void AIAttackState::Enter()
{
	_myEnemy->GetWeapon()->Reset();
}
void AIAttackState::Exit()
{

}
void AIAttackState::Update(float deltaTime)
{	
	StatusEffects currentEffect = _myEnemy->GetCurrentStatusEffects();
	if (currentEffect == STATUS_EFFECT_TIME_STOP)
	{
		_myEnemy->TickDownStatusDuration(deltaTime);
		return;
	}

	_myEnemy->GetWeapon()->Shoot();

	AIBaseState::Update(deltaTime);

}
void AIAttackState::Init()
{

}
int AIAttackState::CheckTransitions()
{
	if (!_controller->CheckIfPlayerIsSeenForEnemy(_myEnemy))
	{
		return AI_STATE_PATROL;
	}
	return AI_STATE_ATTACK;
}
int AIAttackState::GetType()
{
	return AI_STATE_ATTACK;
}

void AIAttackState::OnHit(float damage, StatusEffects effect, float duration)
{
	_myEnemy->ReduceHealth(damage);
	_myEnemy->SetStatusEffects(effect, duration);
}

void AIAttackState::GlobalStatus(StatusEffects effect, float duration)
{
	_myEnemy->SetStatusEffects(effect, duration);
}