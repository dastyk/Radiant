#include "AIBaseState.h"
#include "Shodan.h"

void AIBaseState::Update(float deltaTime)
{
	if (_myEnemy->GetWeapon())
	{
		if (_myEnemy->GetCurrentStatusEffects() != STATUS_EFFECT_CHARMED)
		{
			XMFLOAT3 playerPosition;
			XMStoreFloat3(&playerPosition, _controller->PlayerCurrentPosition());
			_myEnemy->GetWeapon()->Update(_myEnemy->GetEntity(), deltaTime, playerPosition);
		}
		else
		{
			//Fire at closest enemy
			XMFLOAT3 myPosition = _myEnemy->GetCurrentPos();
			XMFLOAT3 closestEnemy = _controller->GetClosestEnemy(myPosition);
			_myEnemy->GetWeapon()->Update(_myEnemy->GetEntity(), deltaTime, closestEnemy);
			_controller->AddPlayerFriendlyProjectiles(_myEnemy);
			
		}
	}
	_myEnemy->Update(deltaTime);
}