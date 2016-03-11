#include "TimeStopper.h"
#include "Shodan.h"
#include "System.h"

TimeStopper::TimeStopper(EntityBuilder* builder, Entity player, List<EnemyWithStates>* _Entities) : Power(builder)
{
	_timeSinceLastActivation = 100;
	_cooldown = 5.0f;
	_enemies = _Entities;
	_powerLevel = 0;
	_duration = 10.0f;
	_type = power_id_t::TIMESTOPPER;

	_powerEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_powerEntity);
	_builder->Bounding()->CreateBoundingSphere(_powerEntity, 0.05f);
	_builder->Light()->BindPointLight(_powerEntity, XMFLOAT3(0, 0, 0), 100.0f, XMFLOAT3(0.0f, 0.0f, 1.0f), 0);
	_builder->Transform()->BindChild(player, _powerEntity);

	_powerName = "Time Stop";
	_description = "Freezes time for all enemies in the dungeon";
}

TimeStopper::~TimeStopper()
{
	_builder->GetEntityController()->ReleaseEntity(_powerEntity);
}

void TimeStopper::Update(Entity playerEntity, float deltaTime)
{
	_timeSinceLastActivation += deltaTime;

	_builder->Light()->ChangeLightIntensity(_powerEntity, max((_duration - _timeSinceLastActivation) * 10, 0.0f));
}

float TimeStopper::Activate(bool& exec, float currentLight)
{
	if (_cooldown - _timeSinceLastActivation <= 0.0f && currentLight >= 4.0f)
	{
		_timeSinceLastActivation = 0.0f;

		for (int i = 0; i < _enemies->Size(); i++)
		{
			_enemies->GetCurrentElement()->_thisEnemyStateController->OnHit(0.0f, STATUS_EFFECT_TIME_STOP, _duration);
			_enemies->MoveCurrent();

			_builder->Light()->ChangeLightIntensity(_powerEntity, _duration);
		}

		exec = true;
		return 4.0f;
	}
	exec = false;
	return 0.0f;
}

bool TimeStopper::Upgrade()
{
	if (_powerLevel <= 5)
	{
		//_damage += 25.0f;
		//_color.y += 0.15f;
		_powerLevel++;
		switch (_powerLevel % 2)
		{
		case 0:
		{
			//_cooldown -= 1.25f;
			return true;
		}
		default:
		{
			//_maxNumbersOfProjectiles++;
			return true;
		}
		}
	}

	return false;
}

