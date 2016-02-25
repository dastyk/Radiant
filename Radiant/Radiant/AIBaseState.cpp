#include "AIBaseState.h"
#include "Shodan.h"

void AIBaseState::Update(float deltaTime)
{
	if (_myEnemy->GetWeapon())
	{
		XMFLOAT3 playerPosition;
		XMStoreFloat3(&playerPosition, _controller->PlayerCurrentPosition());
		_myEnemy->GetWeapon()->Update(_myEnemy->GetEntity(), deltaTime, playerPosition);
		_controller->CheckIfPlayerIsHit(_myEnemy->GetProjectiles());
	}
}