#ifndef _WEAPON_H_
#define _WEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "Projectile.h"
#include "EntityBuilder.h"

class Weapon
{	
protected:
	Weapon(EntityBuilder* builder) : _builder(builder) {}
public:
	virtual void Update(Entity playerEntity, float deltaTime) =0;
	virtual ~Weapon()
	{
		for (int i = 0; i < _projectiles.size(); i++)
		{
			delete _projectiles[i];
		}
	}

	virtual const vector<Projectile*>& GetProjectiles()
	{
		return _projectiles;
	}

	virtual void setActive(bool value)
	{
		_active = value;

		_builder->Light()->ToggleVisible(_weaponEntity, value);
	}

	virtual void Shoot() = 0;

	virtual void AddAmmo() 
	{ 
		_currentAmmo = _currentAmmo + (unsigned int)(_maxAmmo/2.0f); 
		_maxAmmo += max(_currentAmmo - _maxAmmo, 0);
		_builder->Light()->ChangeLightBlobRange(_weaponEntity, 0.1f*(_currentAmmo / (float)_maxAmmo));
	}
	virtual bool HasAmmo() { return _currentAmmo ? true : false; }

protected:
	float _cooldown;
	float _timeSinceLastActivation;
	bool _fire;
	bool _active;
	unsigned int _maxAmmo;
	unsigned int _currentAmmo;
	vector<Projectile*> _projectiles;
	Entity _weaponEntity;
	EntityBuilder* _builder;
};

#endif