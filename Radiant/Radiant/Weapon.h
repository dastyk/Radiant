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
#include "Dungeon.h"
#include "Flags.h"


CCR(Weapons, unsigned int, Num_Weapons = 8,
	Basic = 1 << 0,
	Bounce = 1 << 1,
	Charge = 1 << 2,
	FragBomb = 1 << 3,
	LightThrower = 1 << 4,
	RapidFire = 1 << 5,
	Rocket = 1 << 6,
	Shotgun = 1 << 7);

class Weapon
{	
protected:
	Weapon(EntityBuilder* builder, unsigned int type);
public:




	virtual void Update(const Entity& playerEntity, float deltaTime) =0;
	virtual ~Weapon();

	virtual const vector<Projectile*>& GetProjectiles();

	virtual void setActive(bool value);

	virtual bool Shoot(const Entity& playerEntity) = 0;

	virtual void AddAmmo();
	virtual bool HasAmmo() { return _currentAmmo ? true : false; }
	virtual unsigned int Type() { return _type; };
protected:
	float _cooldown;
	float _timeSinceLastActivation;
	unsigned int _type;
	bool _active;
	XMFLOAT3 _moveVector;
	unsigned int _maxAmmo;
	unsigned int _currentAmmo;
	vector<Projectile*> _projectiles;
	float _currentSize;
	Entity _weaponEntity;
	EntityBuilder* _builder;


	float _uneS;
	float _eS;
};

#endif