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
	ShotgunWeapon(EntityBuilder* builder);
	virtual ~ShotgunWeapon();

	void Update(Entity playerEntity, float deltaTime);


private:
	EntityBuilder* _builder;

	void _Shoot();

};

#endif