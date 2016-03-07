#include "System.h"
#include "EnemyBasicWeapon.h"

EnemyBasicWeapon::EnemyBasicWeapon(EntityBuilder* builder, XMFLOAT3 color) : EnemyWeapon(builder, color)
{
	_timeSinceLastActivation = 100.0f;
	_cooldown = 0.7f;
	_fire = false;

}

EnemyBasicWeapon::~EnemyBasicWeapon()
{
	_builder->GetEntityController()->ReleaseEntity(_weaponEntity);
}

void EnemyBasicWeapon::Reset()
{
	_timeSinceLastActivation = 100.0f;
	_fire = false;
}

void EnemyBasicWeapon::SetCooldownTime(float cooldown)
{
	_cooldown = cooldown;
}

bool EnemyBasicWeapon::Update(Entity thisEntity, float deltaTime, XMFLOAT3 &targetPosition, float damageMultiplier)
{
	_timeSinceLastActivation += deltaTime;
	if (_fire == true)
	{
		_projectiles.push_back(new EnemyBasicProjectile(thisEntity, _builder, _weaponColor, targetPosition, damageMultiplier));
		_fire = false;
		return true;
	}

	return false;

}

bool EnemyBasicWeapon::Shoot()
{
	if (_cooldown - _timeSinceLastActivation <= 0)
	{
		_fire = true;
		
		_timeSinceLastActivation = 0.0;
		return true;
	}

	return false;
}
