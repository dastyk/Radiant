#include "AIMiniGunLighterState.h"
#include "Shodan.h"
#include "EnemyMiniGunWeapon.h"
#include "System.h"

AIMiniGunLightState::AIMiniGunLightState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder) : AIBaseState(currentState, controller, myEnemy, builder)
{
	_myEnemy->SetCurrentWeapon(new EnemyMiniGunWeapon(_builder, myEnemy->GetColor()));
}
AIMiniGunLightState::~AIMiniGunLightState()
{

}
void AIMiniGunLightState::Enter()
{
	_myEnemy->GetWeapon()->Reset();
}
void AIMiniGunLightState::Exit()
{

}
void AIMiniGunLightState::Update(float deltaTime)
{
	StatusEffects currentEffect = _myEnemy->GetCurrentStatusEffects();
	AIBaseState::Update(deltaTime);
	if (currentEffect == STATUS_EFFECT_TIME_STOP)
	{
		return;
	}
	if (!_fireing)
	{
		if (_controller->CheckIfPlayerIsSeenForEnemy(_myEnemy))
		{
			_chargingUp += deltaTime;
			_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), STARTINTENSITYLIGHT + _chargingUp*2.0f);
			if (_chargingUp >= _chargeTime)
			{
				_fireing = true;
			}
		}
		else
		{
			_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), STARTINTENSITYLIGHT);
			_chargingUp = 0.0f;
		}
		return;
	}
	else
	{
		XMVECTOR playerToEnemyVector = XMVector3Normalize(_controller->PlayerCurrentPosition() - _builder->Transform()->GetPosition(_myEnemy->GetEntity()));
		_builder->Transform()->SetDirection(_myEnemy->GetEntity(), playerToEnemyVector);
		if (_myEnemy->GetWeapon()->Shoot())
		{
			float soundVolume = XMVectorGetX(XMVector3Length(playerToEnemyVector));
			if (soundVolume > 0.1f && _timeSinceFireing < 2.0f)
			{
				soundVolume = min(1.0f / soundVolume, 1.0f);
				System::GetAudio()->PlaySoundEffect(L"MiniGunLightAttack.wav", soundVolume);
				_timeSinceFireing = 0.0f;
			}
			_timeSinceFireing += deltaTime;
		}
	}

}
void AIMiniGunLightState::Init()
{

}
int AIMiniGunLightState::CheckTransitions()
{
	if (!_controller->CheckIfPlayerIsSeenForEnemy(_myEnemy) && _myEnemy->GetCurrentStatusEffects() != STATUS_EFFECT_CHARMED)
	{
		return AI_STATE_PATROL;
	}
	return AI_STATE_ATTACK;
}
int AIMiniGunLightState::GetType()
{
	return AI_STATE_ATTACK;
}

void AIMiniGunLightState::OnHit(float damage, StatusEffects effect, float duration)
{
	_myEnemy->ReduceHealth(damage);
	_myEnemy->SetStatusEffects(effect, duration);
}

void AIMiniGunLightState::GlobalStatus(StatusEffects effect, float duration)
{
	_myEnemy->SetStatusEffects(effect, duration);
}

void AIMiniGunLightState::SetDamageModifier(float amount)
{
	_myEnemy->SetDamageMultiplier(amount);
}

void AIMiniGunLightState::AddToDamageModifier(float amount)
{
	_myEnemy->AddToDamageMultiplier(amount);
}

void AIMiniGunLightState::OnEnemyDeath()
{
	_myEnemy->AddToDamageMultiplier(damageModificationPerDeath);
	_myEnemy->AddToSpeedMofication(speedMoficationPerDeath);
}