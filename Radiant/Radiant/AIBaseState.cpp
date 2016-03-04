#include "AIBaseState.h"
#include "Shodan.h"


AIBaseState::~AIBaseState()
{
}

void AIBaseState::Update(float deltaTime)
{
	if (_myEnemy->GetWeapon())
	{
		if (_myEnemy->GetCurrentStatusEffects() != STATUS_EFFECT_CHARMED)
		{
			XMFLOAT3 playerPosition;
			XMStoreFloat3(&playerPosition, _controller->PlayerCurrentPosition());
			_myEnemy->GetWeapon()->Update(_myEnemy->GetEntity(), deltaTime, playerPosition, _myEnemy->GetDamageMultiplier());
		}
		else
		{
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
				_myEnemy->GetWeapon()->Update(_myEnemy->GetEntity(), deltaTime, enemyLocation, _myEnemy->GetDamageMultiplier());
				_controller->AddPlayerFriendlyProjectiles(_myEnemy);
			}

		}
	}
	_myEnemy->TickDownStatusDuration(deltaTime);
	_myEnemy->Update(deltaTime);
}

AIBaseState::~AIBaseState()
{
	_controller = nullptr;
	_builder = nullptr;
	_myEnemy = nullptr;
}