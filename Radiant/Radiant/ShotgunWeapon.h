#ifndef _SHOTGUN_WEAPON_H_
#define _SHOTGUN_WEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "Weapon.h"
#include "ShotgunProjectile.h"

class ShotgunWeapon : public Weapon
{

public:
	ShotgunWeapon(EntityBuilder* builder, Entity player);
	virtual ~ShotgunWeapon();

	void Update(const Entity& playerEntity, float deltaTime);
	bool Shoot(const Entity& playerEntity);
};

#endif