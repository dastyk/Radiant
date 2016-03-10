#ifndef _BASIC_WEAPON_H_
#define _BASIC_WEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "Weapon.h"
#include "BasicProjectile.h"

class BasicWeapon: public Weapon
{

public:
	BasicWeapon(EntityBuilder* builder, Entity player);
	virtual ~BasicWeapon();

	void Update(const Entity& playerEntity, float deltaTime);
	bool Shoot(const Entity& playerEntity);
	bool HasAmmo();

};

#endif