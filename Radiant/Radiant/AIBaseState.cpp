#include "AIBaseState.h"
#include "Shodan.h"


AIBaseState::~AIBaseState()
{
	_controller = nullptr;
	_builder = nullptr;
	_myEnemy = nullptr;
}
void AIBaseState::Update(float deltaTime)
{
	if (_myEnemy->GetWeapon())
	{
		if (_myEnemy->GetCurrentStatusEffects() != STATUS_EFFECT_CHARMED)
		{
			XMFLOAT3 playerPosition;
			XMStoreFloat3(&playerPosition, _controller->PlayerCurrentPosition());
			playerPosition.y = 0.2f;
			_myEnemy->GetWeapon()->Update(_myEnemy->GetEntity(), deltaTime, playerPosition, _myEnemy->GetDamageMultiplier());
		}
		else
		{
			Enemy* test = _myEnemy->GetClosestEnemy();
			if (_myEnemy->GetClosestEnemy() == nullptr)
			{
				//Fire at closest enemy
				XMFLOAT3 myPosition = _myEnemy->GetCurrentPos();
				_myEnemy->SetClosestEnemy(_controller->GetClosestEnemy(_myEnemy->GetEntity()));
			}
			else
			{
				XMFLOAT3 enemyLocation;
				XMStoreFloat3(&enemyLocation, _builder->Transform()->GetPosition(_myEnemy->GetClosestEnemy()->GetEntity()));
				if (enemyLocation.x == 0.0f && enemyLocation.y == 0.0f && enemyLocation.z == 0.0f)
				{
					_myEnemy->SetClosestEnemy(nullptr);
				}
				else
				{
					_myEnemy->GetWeapon()->Update(_myEnemy->GetEntity(), deltaTime, enemyLocation, _myEnemy->GetDamageMultiplier());
					_controller->AddPlayerFriendlyProjectiles(_myEnemy);
				}
			}

		}
	}
	if (_beenHit)
	{
		_glowOnHitTimer -= deltaTime;
		if (_glowOnHitTimer < 0.0f)
		{
			_glowOnHitTimer = 0.0f;
			_beenHit = false;
		}
		_builder->Light()->ChangeLightIntensity(_myEnemy->GetEntity(), _resetIntensity*(1 + 250*_glowOnHitTimer));
	}
	_myEnemy->TickDownStatusDuration(deltaTime);
	_myEnemy->Update(deltaTime);
}

void AIBaseState::ThisOneDied()
{
	_controller->EnemyDied();
}
