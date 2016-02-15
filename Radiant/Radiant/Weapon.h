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

protected:
	float _cooldown;
	float _timeSinceLastActivation;
	bool _fire;
	
	vector<Projectile*> _projectiles;
	Entity _weaponEntity;
	EntityBuilder* _builder;
};

#endif