#include "System.h"
#include "EnemyMiniGunWeapon.h"

EnemyMiniGunWeapon::EnemyMiniGunWeapon(EntityBuilder* builder, XMFLOAT3 color) : EnemyWeapon(builder, color)
{
	_timeSinceLastActivation = 100.0f;
	_angle = 0.0f;
	_cooldown = 0.1f;
	_fire = false;

}

EnemyMiniGunWeapon::~EnemyMiniGunWeapon()
{

}

void EnemyMiniGunWeapon::Reset()
{
	_timeSinceLastActivation = 100.0f;
	_fire = false;
}

void EnemyMiniGunWeapon::SetCooldownTime(float cooldown)
{
	_cooldown = cooldown;
}

bool EnemyMiniGunWeapon::Update(Entity thisEntity, float deltaTime, XMFLOAT3 &targetPosition, float damageMultiplier)
{
	_timeSinceLastActivation += deltaTime;
	targetPosition.x += cos(_angle)*(rand() % 2);
	targetPosition.y += sin(_angle)*(rand() % 2);
	_angle += XM_PI*deltaTime;
	if (_angle > XM_2PI)
	{
		_angle -= XM_2PI;
	}
	

	if (_fire)
	{
		_projectiles.push_back(new EnemyMiniGunProjectile(thisEntity, _builder, _weaponColor, targetPosition, damageMultiplier));

		_fire = false;
	}

	return true;
}

bool EnemyMiniGunWeapon::Shoot()
{
	if (_cooldown - _timeSinceLastActivation <= 0)
	{
		_fire = true;

		_timeSinceLastActivation = 0.0;
		return true;
	}
	return false;
}
