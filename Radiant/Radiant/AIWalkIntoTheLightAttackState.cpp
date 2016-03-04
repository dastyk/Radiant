#include "AIWalkIntoTheLightAttackState.h"
#include "Shodan.h"
#include "EnemyBasicWeapon.h"
#include "System.h"

AIWalkIntoTheLightAttackState::AIWalkIntoTheLightAttackState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder) : AIBaseState(currentState, controller, myEnemy, builder)
{
	_myEnemy->SetCurrentWeapon(nullptr);
	_movementVector = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_myIntensity = -5.0f;
	_timeBeforeUpdate = 0.1f;
	_timeSinceUpdate = 0.0f;
	_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), _myIntensity);
}
AIWalkIntoTheLightAttackState::~AIWalkIntoTheLightAttackState()
{
	_timeSinceUpdate = 0.0f;
}
void AIWalkIntoTheLightAttackState::Enter()
{

}
void AIWalkIntoTheLightAttackState::Exit()
{

}
void AIWalkIntoTheLightAttackState::Update(float deltaTime)
{
	StatusEffects currentEffect = _myEnemy->GetCurrentStatusEffects();
	AIBaseState::Update(deltaTime);
	if (currentEffect == STATUS_EFFECT_TIME_STOP)
	{
		return;
	}
	_timeSinceUpdate += deltaTime;
	XMFLOAT3 myPosition = _myEnemy->GetCurrentPos();

	if (_timeSinceUpdate > _timeBeforeUpdate)
	{
		XMFLOAT3 playerPosition;
		XMStoreFloat3(&playerPosition,_controller->PlayerCurrentPosition());
		XMFLOAT3 myPosition = _myEnemy->GetCurrentPos();
		XMStoreFloat3(&_movementVector, XMVector3Normalize(XMVectorSet(playerPosition.x - myPosition.x, 0.0f, playerPosition.z - myPosition.z, 0.0f)));
		_timeSinceUpdate = 0.0f;
	}

	myPosition.x += _movementVector.x*_myEnemy->GetSpeedModification()*deltaTime;
	myPosition.z = _movementVector.z*_myEnemy->GetSpeedModification()*deltaTime;

	if (_controller->NodeWalkable(myPosition.x, myPosition.z))
	{
		_builder->Transform()->SetPosition(_myEnemy->GetEntity(), myPosition);
	}



}
void AIWalkIntoTheLightAttackState::Init()
{

}
int AIWalkIntoTheLightAttackState::CheckTransitions()
{
	if (!_controller->CheckIfPlayerIsSeenForEnemy(_myEnemy))
	{
		return AI_STATE_PATROL;
	}
	return AI_STATE_ATTACK;
}
int AIWalkIntoTheLightAttackState::GetType()
{
	return AI_STATE_ATTACK;
}

void AIWalkIntoTheLightAttackState::OnHit(float damage, StatusEffects effect, float duration)
{
	_myEnemy->ReduceHealth(damage);
	_myEnemy->SetStatusEffects(effect, duration);
}

void AIWalkIntoTheLightAttackState::GlobalStatus(StatusEffects effect, float duration)
{
	_myEnemy->SetStatusEffects(effect, duration);
}

void AIWalkIntoTheLightAttackState::SetDamageModifier(float amount)
{
	_myEnemy->SetDamageMultiplier(amount);
}

void AIWalkIntoTheLightAttackState::AddToDamageModifier(float amount)
{
	_myEnemy->AddToDamageMultiplier(amount);
}

void AIWalkIntoTheLightAttackState::OnEnemyDeath()
{
	_myEnemy->AddToDamageMultiplier(damageModificationPerDeath);
	_myEnemy->AddToSpeedMofication(speedMoficationPerDeath);
	_myIntensity -= 1.0f;
	_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), _myIntensity); 
	float newSize = STARTBLOBRANGELIGHT *0.3f;
	float newRange = STARTRANGELIGHT*(-_myIntensity);

	_builder->Light()->ChangeLightRange(_myEnemy->GetEntity() , newRange);
	_builder->Transform()->SetScale(_myEnemy->GetEntity(), XMFLOAT3(newSize, newSize, newSize));
	_builder->Light()->ChangeLightBlobRange(_myEnemy->GetEntity(), newSize);

}