#include "AIAttackState.h"
#include "Shodan.h"


AIAttackState::AIAttackState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder) : AIBaseState(currentState, controller, myEnemy, builder)
{

}
AIAttackState::~AIAttackState()
{

}
void AIAttackState::Enter()
{

}
void AIAttackState::Exit()
{

}
void AIAttackState::Update(float deltaTime)
{

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
