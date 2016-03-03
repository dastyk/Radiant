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
	while (_myPath == nullptr)
	{
		_myPath = _controller->NeedPath(myEntity);
	}
	while (_myPath->nrOfNodes < 11)
	{
		SAFE_DELETE(_myPath);
		_myPath = _controller->NeedPath(myEntity);
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
	if (currentEffect == STATUS_EFFECT_TIME_STOP)
	{
		AIBaseState::Update(deltaTime);
		return;
	}

	if (_arrived)
	{
		_timer += deltaTime;
		if (_timer > _waitTime)
		{
			_myEnemy->GetWeapon()->Shoot();
			AIBaseState::Update(deltaTime);

			Entity a = _myEnemy->GetEntity();
			XMVECTOR currentPos = _builder->GetEntityController()->Transform()->GetPosition(a);
			_builder->Transform()->SetPosition(_myEnemy->GetEntity(), _currentGoal);
			float soundVolume = XMVectorGetX(XMVector3Length(XMLoadFloat3(&_currentGoal) - _controller->PlayerCurrentPosition()));
			if (soundVolume > 0.1f)
			{
				soundVolume = min(1.0f / soundVolume, 1.0f);
				System::GetAudio()->PlaySoundEffect(L"EnemyTeleport.wav", soundVolume);
			}

			while (_myPath->nrOfNodes < _nrOfStepsTaken + 10)
			{
				SAFE_DELETE(_myPath);
				_myPath = _controller->NeedPath(_myEnemy->GetEntity());
				_nrOfStepsTaken = 0;
			}
			_currentGoal = XMFLOAT3(_myPath->nodes[_nrOfStepsTaken + 10].x + _myPath->nodes[_nrOfStepsTaken + 10].offsetX, 0.5f, _myPath->nodes[_nrOfStepsTaken + 10].y + _myPath->nodes[_nrOfStepsTaken + 10].offsetY);
			_nrOfStepsTaken += 5;

			_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), _originalIntensity);
			_arrived = false;
		}
		else
		{
			_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), min(_originalIntensity, _originalIntensity*(_waitTime - _timer + 0.2f)));
			AIBaseState::Update(deltaTime);
		}
	}
	else
	{
		AIBaseState::Update(deltaTime);
		_timer -= 2*deltaTime;
		if (_timer < 0.0f)
		{
			_timer = 0.0f;
			_arrived = true;
		}
		_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), max(_originalIntensity, _originalIntensity*(_waitTime - _timer + 0.2f)));
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
	_myEnemy->SetStatusEffects(effect, duration);
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