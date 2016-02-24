#include "AIAttackState.h"
#include "Shodan.h"
#include "EnemyBasicWeapon.h"

AIAttackState::AIAttackState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder) : AIBaseState(currentState, controller, myEnemy, builder)
{
	_myEnemy->SetCurrentWeapon(new EnemyBasicWeapon(_builder, myEnemy->GetColor()));
	_myEnemy->GetWeapon()->SetCooldownTime((1.0f / (rand() % 13))*3.0f);
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
	AIBaseState::Update(deltaTime);

	_myEnemy->GetWeapon()->Shoot();


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
