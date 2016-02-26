#include "System.h"
#include "EnemyDeathBlossomWeapon.h"

EnemyDeathBlossomWeapon::EnemyDeathBlossomWeapon(EntityBuilder* builder, XMFLOAT3 color) : EnemyWeapon(builder, color)
{
	_timeSinceLastActivation = 100.0f;
	_cooldown = 0.0f;
	_fire = false;

}

EnemyDeathBlossomWeapon::~EnemyDeathBlossomWeapon()
{

}

void EnemyDeathBlossomWeapon::Reset()
{
	_timeSinceLastActivation = 100.0f;
	_fire = false;
}

void EnemyDeathBlossomWeapon::SetCooldownTime(float cooldown)
{
	_cooldown = cooldown;
}

bool EnemyDeathBlossomWeapon::Update(Entity playerEntity, float deltaTime, XMFLOAT3 &playerPosition)
{
	_timeSinceLastActivation += deltaTime;

	if (_fire)
	{
		for (int i = 0; i < 36; i++)
		{
			_projectiles.push_back(new EnemyDeathBlossomProjectile(playerEntity, _builder, _weaponColor, playerPosition, 10 * i));
		}
		_fire = false;
	}

	return true;
}

void EnemyDeathBlossomWeapon::Shoot()
{
	_fire = true;
}
