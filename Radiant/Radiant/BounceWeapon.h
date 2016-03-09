#ifndef _BOUNCE_WEAPON_H_
#define _BOUNCE_WEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "Weapon.h"
#include "BounceProjectile.h"

class BounceWeapon : public Weapon
{

public:
	BounceWeapon(EntityBuilder* builder, Entity player);
	virtual ~BounceWeapon();

	void Update(const Entity& playerEntity, float deltaTime);
	bool Shoot(const Entity& playerEntity);

};

#endif