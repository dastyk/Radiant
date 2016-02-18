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
	BasicWeapon(EntityBuilder* builder);
	virtual ~BasicWeapon();

	void Update(Entity playerEntity, float deltaTime);


private:
	void _Shoot();

};

#endif