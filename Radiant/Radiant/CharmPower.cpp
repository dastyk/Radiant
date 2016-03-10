#include "CharmPower.h"
#include "Shodan.h"
#include "System.h"

CharmPower::CharmPower(EntityBuilder* builder, Entity player, List<EnemyWithStates>* _Entities) : Power(builder)
{
	_timeSinceLastActivation = 100;
	_cooldown = 5.0f;
	_enemies = _Entities;
	_powerLevel = 0;
	_active = false;
	_duration = 5.0f;

	_powerName = "Charm Enemy";
	_description = "Shoot a projectile, if it hits an enemy it will be charmed and fight for you (for a while at least)";
}

CharmPower::~CharmPower()
{
	_builder->GetEntityController()->ReleaseEntity(_powerEntity);

	if(_active)
		_builder->GetEntityController()->ReleaseEntity(_projectile);
	
}

void CharmPower::Update(Entity playerEntity, float deltaTime)
{
	_timeSinceLastActivation += deltaTime;

	if (_active)
	{
		_builder->Transform()->MoveForward(_projectile, 15 * deltaTime);

		if (_builder->Bounding()->CheckCollision(_projectile, _enemies->GetCurrentElement()->_thisEnemy->GetEntity()))
		{
			_builder->GetEntityController()->ReleaseEntity(_projectile);
			_enemies->GetCurrentElement()->_thisEnemyStateController->OnHit(0.0f, STATUS_EFFECT_CHARMED, _duration);
			_active = false;
			
		}
	}
	else if (_justFired)
	{
		_justFired = false;
		_projectile = _builder->EntityC().Create();
		_builder->Transform()->CreateTransform(_projectile);

		XMFLOAT3 temp;
		XMStoreFloat3(&temp, _builder->Transform()->GetPosition(playerEntity));
		_builder->Transform()->SetPosition(_projectile, temp);
		XMStoreFloat3(&temp, _builder->Transform()->GetRotation(playerEntity));
		_builder->Transform()->SetRotation(_projectile, temp);

		_builder->Bounding()->CreateBoundingSphere(_projectile, 0.25f);
		_builder->Light()->BindPointLight(_projectile, XMFLOAT3(0, 0, 0), 0.25f, XMFLOAT3(1.0f, 105.0f / 255.0f, 180.0f / 255.0f), 10.0f);
		_builder->Transform()->MoveDown(_projectile, 0.0f);
	}

}

float CharmPower::Activate(bool& exec, float currentLight)
{
	if (_cooldown - _timeSinceLastActivation <= 0.0f && currentLight >= 4.0f)
	{
		_timeSinceLastActivation = 0.0f;
		_justFired = true;

		exec = true;
		return 4.0f;
	}
	exec = false;
	return 0.0f;
}

bool CharmPower::Upgrade()
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

