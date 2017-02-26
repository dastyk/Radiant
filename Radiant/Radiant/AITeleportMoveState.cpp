#include "AITeleportMoveState.h"
#include "Shodan.h"
#include "System.h"

AITeleportMoveState::AITeleportMoveState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder) : AIBaseState(currentState, controller, myEnemy, builder)
{
	_myPath = nullptr;
	_waitTime = (rand() % 6 + 1)*1.2f + 1.5f;
	_timer = 0.0f;
	_originalIntensity = _builder->Light()->GetLightIntensity(_myEnemy->GetEntity());
	_nrOfStepsTaken = 0;
	_arrived = true;
	_myEnemy->SetHealth(50.0f);
}

AITeleportMoveState::~AITeleportMoveState()
{
	SAFE_DELETE(_myPath);
}

void AITeleportMoveState::Enter()
{
	Entity myEntity = _myEnemy->GetEntity();
	_myPath = nullptr;
	if(_myPath == nullptr)
	{
		_controller->EnemyStuck(_myEnemy->GetEntity());
		SAFE_DELETE(_myPath);
		_myPath = _controller->NeedPath(_myEnemy->GetEntity());
	}
	if (_myPath == nullptr)
		return;
	while(_myPath->nrOfNodes < 11)
	{
		SAFE_DELETE(_myPath);
		_myPath = _controller->NeedPath(myEntity);
		if (_myPath == nullptr)
			return;
	}

	_nrOfStepsTaken = 0;
	_currentGoal = XMFLOAT3(_myPath->nodes[_nrOfStepsTaken + 10].x + _myPath->nodes[_nrOfStepsTaken + 10].offsetX, 0.5f, _myPath->nodes[_nrOfStepsTaken + 10].y + _myPath->nodes[_nrOfStepsTaken + 10].offsetY);
	_myEnemy->SetCurrentWeapon(new EnemyDeathBlossomWeapon(_builder, _myEnemy->GetColor()));
}

void AITeleportMoveState::Exit()
{
	SAFE_DELETE(_myPath);
}

void AITeleportMoveState::Update(float deltaTime)
{	
	StatusEffects currentEffect = _myEnemy->GetCurrentStatusEffects();
	if (currentEffect == STATUS_EFFECT_TIME_STOP || _myPath == nullptr)
	{
		if (_myPath == nullptr)
		{
			_controller->EnemyStuck(_myEnemy->GetEntity());
			_myPath = _controller->NeedPath(_myEnemy->GetEntity());
			return;
		}
		AIBaseState::Update(deltaTime);
		return;
	}

	if (_arrived)
	{
		_timer += deltaTime;
		if (_timer > _waitTime)
		{
			_resetIntensity = _originalIntensity;
			_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), _originalIntensity);
			_myEnemy->GetWeapon()->Shoot();
			AIBaseState::Update(deltaTime);

			Entity a = _myEnemy->GetEntity();
			XMVECTOR currentPos = _builder->GetEntityController()->Transform()->GetPosition(a);
			_builder->Transform()->SetPosition(_myEnemy->GetEntity(), _currentGoal);
			float soundVolume = XMVectorGetX(XMVector3Length(XMLoadFloat3(&_currentGoal) - _controller->PlayerCurrentPosition()));
			if (soundVolume > 0.1f)
			{
				soundVolume = min(1.0f / soundVolume, 1.0f);
//				System::GetAudio()->PlaySoundEffect(L"EnemyTeleport.wav", soundVolume);
			}

			while (_myPath->nrOfNodes < _nrOfStepsTaken + 10)
			{
				SAFE_DELETE(_myPath);
				_myPath = _controller->NeedPath(_myEnemy->GetEntity());
				_nrOfStepsTaken = 0;
			}
			_currentGoal = XMFLOAT3(_myPath->nodes[_nrOfStepsTaken + 10].x + _myPath->nodes[_nrOfStepsTaken + 10].offsetX, 0.5f, _myPath->nodes[_nrOfStepsTaken + 10].y + _myPath->nodes[_nrOfStepsTaken + 10].offsetY);
			_nrOfStepsTaken += 5;
			_arrived = false;
		}
		else
		{
			_resetIntensity = max(_originalIntensity, _originalIntensity*(_waitTime - _timer + 0.2f));
			_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), _resetIntensity);
			AIBaseState::Update(deltaTime);
		}
	}
	else
	{
		_timer -= 2*deltaTime;
		if (_timer < 0.0f)
		{
			_timer = 0.0f;
			_arrived = true;
		}
		_resetIntensity = max(_originalIntensity, _originalIntensity*(_waitTime - _timer + 0.2f));
		_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), _resetIntensity);
		AIBaseState::Update(deltaTime);
	}
}

void AITeleportMoveState::Init()
{

}

int AITeleportMoveState::CheckTransitions()
{
	//The teleport enemy cannot attack, but will only teleport around and be cool
	return AI_STATE_PATROL;
}

int AITeleportMoveState::GetType()
{
	return AI_STATE_PATROL;
}

void AITeleportMoveState::OnHit(float damage, StatusEffects effect, float duration)
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

void AITeleportMoveState::GlobalStatus(StatusEffects effect, float duration)
{
	_myEnemy->SetStatusEffects(effect, duration);
}

void AITeleportMoveState::SetDamageModifier(float amount)
{
	_myEnemy->SetDamageMultiplier(amount);
}

void AITeleportMoveState::AddToDamageModifier(float amount)
{
	_myEnemy->AddToDamageMultiplier(amount);
}

void AITeleportMoveState::OnEnemyDeath()
{
	_myEnemy->AddToDamageMultiplier(damageModificationPerDeath);
	_myEnemy->AddToSpeedMofication(speedMoficationPerDeath);
	_waitTime -= speedMoficationPerDeath;
	if (_waitTime < 1.0f)
		_waitTime = 1.0f;
}