#ifndef _BASICWEAPON_H_
#define _BASICWEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "System.h"
#include "Weapon.h"

class BasicWeapon: public Weapon
{

public:
	BasicWeapon();
	~BasicWeapon();

	void Update(float deltaTime);


private:
	//float _cooldown;

	//Projectile* projectile;
	//Entity weaponEntity;

};

#endif