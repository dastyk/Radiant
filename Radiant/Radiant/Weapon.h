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
	virtual void Update(float deltaTime) =0;


protected:
	float _cooldown;
	float _timeSinceLastActivation;
	
	vector<Projectile> _projectiles;
	Entity _weaponEntity;

};

#endif