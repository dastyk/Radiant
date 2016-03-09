#ifndef _ROCKET_WEAPON_H_
#define _ROCKET_WEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "Weapon.h"
#include "RocketProjectile.h"

class RocketWeapon : public Weapon
{

public:
	RocketWeapon(EntityBuilder* builder, Entity player);
	virtual ~RocketWeapon();

	void Update(Entity playerEntity, float deltaTime);
	void Shoot();

private:
	void _Shoot();

};

#endif