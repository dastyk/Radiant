#include "BasicWeapon.h"

BasicWeapon::BasicWeapon()
{
	_timeSinceLastActivation = 100;
	_cooldown = 1;
}

BasicWeapon::~BasicWeapon()
{

}

void BasicWeapon::Update(float deltaTime)
{
	_timeSinceLastActivation += deltaTime;

	if (System::GetInput()->IsMouseKeyDown(VK_LBUTTON) && _cooldown - _timeSinceLastActivation <= 0)
	{
		// create projectile

		// play sound
	}
}