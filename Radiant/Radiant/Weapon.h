#ifndef _WEAPON_H_
#define _WEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "System.h"
#include "Projectile.h"

class Weapon
{	

public:
	virtual void Update() =0;


protected:
	float _cooldown;
	
	vector<Projectile> _projectiles;
	Entity _weaponEntity;

};

#endif