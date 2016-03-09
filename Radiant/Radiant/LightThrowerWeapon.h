#ifndef _LIGHT_THROWER_WEAPON_H_
#define _LIGHT_THROWER_WEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "Weapon.h"
#include "LightThrowerProjectile.h"

class LightThrowerWeapon : public Weapon
{

public:
	LightThrowerWeapon(EntityBuilder* builder, Entity player);
	virtual ~LightThrowerWeapon();

	void Update(Entity playerEntity, float deltaTime);
	void Shoot();

private:
	void _Shoot();

};

#endif