#ifndef _BASICWEAPON_H_
#define _BASICWEAPON_H_

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
	EntityBuilder* _builder;

	void _Shoot();

};

#endif