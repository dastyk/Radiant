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
	Weapon(EntityBuilder* builder);
public:
	virtual void Update(Entity playerEntity, float deltaTime) =0;
	virtual ~Weapon();

	virtual const vector<Projectile*>& GetProjectiles();

	virtual void setActive(bool value);

	virtual void Shoot() = 0;

	virtual void AddAmmo();
	virtual bool HasAmmo() { return _currentAmmo ? true : false; }

protected:
	float _cooldown;
	float _timeSinceLastActivation;
	bool _fire;
	bool _active;
	XMFLOAT3 _moveVector;
	unsigned int _maxAmmo;
	unsigned int _currentAmmo;
	vector<Projectile*> _projectiles;
	float _currentSize;
	Entity _weaponEntity;
	EntityBuilder* _builder;
};

#endif