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
			_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), STARTINTENSITYLIGHT + _chargingUp*2.0f);  //SHALL NOT BE STARTINTENSITYLIGHT
			if (_chargingUp >= _chargeTime)
			{
				_fireing = true;
			}

			XMVECTOR playerPos = _controller->PlayerCurrentPosition();
			XMVECTOR myPos = XMLoadFloat3(&_myEnemy->GetCurrentPos());
			XMVECTOR temp = XMVectorSubtract(playerPos, myPos);
			temp = XMVector3Normalize(temp);

			float differenceX = XMVectorGetX(temp);

			if (XMVectorGetZ(temp) >= 0.0f)
			{
				_builder->Transform()->SetRotation(_myEnemy->GetEntity(), XMFLOAT3(0.0f, 90 * differenceX, 0.0f));
				_builder->Transform()->MoveForward(_myEnemy->GetEntity(), 0.0f);
			}
			else
			{
				_builder->Transform()->SetRotation(_myEnemy->GetEntity(), XMFLOAT3(0.0f, 180.0f - differenceX * 90, 0.0f));
				_builder->Transform()->MoveForward(_myEnemy->GetEntity(), 0.0f);
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
		XMVECTOR playerToEnemyVector = _controller->PlayerCurrentPosition() - _builder->Transform()->GetPosition(_myEnemy->GetEntity());
		if (_myEnemy->GetWeapon()->Shoot())
		{
			float soundVolume = XMVectorGetX(XMVector3Length(playerToEnemyVector));
			_timeSinceFireing += deltaTime;
			if (soundVolume > 0.1f && _timeSinceFireing < 2.0f)
			{
				soundVolume = min(1.0f / soundVolume, 1.0f);
				System::GetAudio()->PlaySoundEffect(L"MiniGunLightAttack.wav", soundVolume);
				_timeSinceFireing = 0.0f;
			}
		}

		XMVECTOR playerPos = _controller->PlayerCurrentPosition(); // SPINNING UUUP!
		XMVECTOR myPos = XMLoadFloat3(&_myEnemy->GetCurrentPos());
		XMVECTOR temp = XMVectorSubtract(playerPos, myPos);
		temp = XMVector3Normalize(temp);

		float differenceX = XMVectorGetX(temp);

		if (XMVectorGetZ(temp) >= 0.0f)
		{
			_builder->Transform()->SetRotation(_myEnemy->GetEntity(), XMFLOAT3(0.0f, 90 * differenceX, 0.0f));
			_builder->Transform()->MoveForward(_myEnemy->GetEntity(), 0.0f);
		}
		else
		{
			_builder->Transform()->SetRotation(_myEnemy->GetEntity(), XMFLOAT3(0.0f, 180.0f - differenceX * 90, 0.0f));
			_builder->Transform()->MoveForward(_myEnemy->GetEntity(), 0.0f);
		}

	}

}
void AIMiniGunLightState::Init()
{

}
int AIMiniGunLightState::CheckTransitions()
{
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
	if (!_beenHit)
	{
		_beenHit = true;
		_resetIntensity = _builder->Light()->GetLightIntensity(_myEnemy->GetEntity());
		_glowOnHitTimer = 0.5f;
		_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), _resetIntensity*(1000 * 0.5f + 1));
	}
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