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

	void Update(const Entity& playerEntity, float deltaTime);
	bool Shoot(const Entity& playerEntity);


};

#endif