#include "System.h"
#include "EnemyBasicWeapon.h"

EnemyBasicWeapon::EnemyBasicWeapon(EntityBuilder* builder, XMFLOAT3 color) : EnemyWeapon(builder, color)
{
	_timeSinceLastActivation = 100.0f;
	_cooldown = 0.4f;
	_fire = false;

}

EnemyBasicWeapon::~EnemyBasicWeapon()
{

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

bool EnemyBasicWeapon::Update(Entity playerEntity, float deltaTime, XMFLOAT3 &playerPosition)
{
	_timeSinceLastActivation += deltaTime;

	for (int i = 0; i < _projectiles.size(); i++)
	{
		_projectiles[i]->Update(deltaTime);
	}

	for (int i = 0; i < _projectiles.size(); i++)
	{
		if (!_projectiles[i]->GetState())
		{
			delete _projectiles[i];
			_projectiles.erase(_projectiles.begin() + i);
			_projectiles.shrink_to_fit();
			i--;
		}
	}
	if (_fire == true)
	{
		_projectiles.push_back(new EnemyBasicProjectile(playerEntity, _builder, _weaponColor, playerPosition));
		_fire = false;
		return true;
	}

	return false;

}

void EnemyBasicWeapon::Shoot()
{
	if (_cooldown - _timeSinceLastActivation <= 0)
	{
		_fire = true;
		
		_timeSinceLastActivation = 0.0;
	}
}
