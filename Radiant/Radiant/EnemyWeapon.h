#ifndef _ENEMY_WEAPON_H_
#define _ENEMY_WEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "Projectile.h"
#include "EntityBuilder.h"

class EnemyWeapon
{
protected:
	EnemyWeapon(EntityBuilder* builder, XMFLOAT3 color) : _builder(builder), _weaponColor(color) {}
public:
	virtual bool Update(Entity playerEntity, float deltaTime, XMFLOAT3 &targetPosition, float damageMultiplier) = 0;
	virtual ~EnemyWeapon()
	{
		for (int i = 0; i < _projectiles.size(); i++)
		{
			delete _projectiles[i];
		}
	}

	virtual vector<Projectile*> GetProjectiles()
	{
		return _projectiles;
	}

	virtual vector<Projectile*> GetProjectilesOwnership()
	{
		vector<Projectile*> projectilesToReturn;
		while (_projectiles.size())
		{
			projectilesToReturn.push_back(_projectiles.back());
			_projectiles.pop_back();
		}
		return projectilesToReturn;
	}

	virtual void Shoot() = 0;
	virtual void Reset() = 0;
	virtual void SetCooldownTime(float cooldown) { _cooldown = cooldown; };
protected:
	float _cooldown;
	float _timeSinceLastActivation;
	bool _fire;

	XMFLOAT3 _weaponColor;
	vector<Projectile*> _projectiles;
	Entity _weaponEntity;
	EntityBuilder* _builder;
};

#endif