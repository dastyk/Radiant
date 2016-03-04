#ifndef _RAPID_FIRE_WEAPON_H_
#define _RAPID_FIRE_WEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "Weapon.h"
#include "RapidFireProjectile.h"

class RapidFireWeapon : public Weapon
{

public:
	RapidFireWeapon(EntityBuilder* builder, Entity player);
	virtual ~RapidFireWeapon();

	void Update(Entity playerEntity, float deltaTime);
	void Shoot();

private:
	void _Shoot();

};

#endif