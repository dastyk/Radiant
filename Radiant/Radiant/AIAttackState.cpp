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
	AIBaseState::Update(deltaTime);
	if (currentEffect == STATUS_EFFECT_TIME_STOP)
	{
		return;
	}

	_myEnemy->GetWeapon()->Shoot();


}
void AIAttackState::Init()
{

}
int AIAttackState::CheckTransitions()
{
	if (!_controller->CheckIfPlayerIsSeenForEnemy(_myEnemy) && _myEnemy->GetCurrentStatusEffects() != STATUS_EFFECT_CHARMED)
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