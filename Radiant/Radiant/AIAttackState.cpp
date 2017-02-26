#include "AIAttackState.h"
#include "Shodan.h"
#include "EnemyBasicWeapon.h"
#include "System.h"

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

	if (_myEnemy->GetWeapon()->Shoot())
	{
		float soundVolume = XMVectorGetX(XMVector3Length(_builder->Transform()->GetPosition(_myEnemy->GetEntity()) - _controller->PlayerCurrentPosition()));
		if (soundVolume > 0.1f)
		{
			soundVolume = min(1.0f / soundVolume, 1.0f);
//			System::GetAudio()->PlaySoundEffect(L"EnemyNormalTypeAttack.wav", soundVolume);
		}
	}

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

	if(effect != STATUS_EFFECT_NORMAL)
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

void AIAttackState::GlobalStatus(StatusEffects effect, float duration)
{
	_myEnemy->SetStatusEffects(effect, duration);
}

void AIAttackState::SetDamageModifier(float amount)
{
	_myEnemy->SetDamageMultiplier(amount);
}

void AIAttackState::AddToDamageModifier(float amount)
{
	_myEnemy->AddToDamageMultiplier(amount);
}

void AIAttackState::OnEnemyDeath()
{
	_myEnemy->AddToDamageMultiplier(damageModificationPerDeath);
	_myEnemy->AddToSpeedMofication(speedMoficationPerDeath);
}