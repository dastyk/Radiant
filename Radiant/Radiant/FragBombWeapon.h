#ifndef _FRAG_BOMB_WEAPON_H_
#define _FRAG_BOMB_WEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "Weapon.h"
#include "FragBombProjectile.h"

class FragBombWeapon : public Weapon
{

public:
	FragBombWeapon(EntityBuilder* builder);
	virtual ~FragBombWeapon();

	void Update(Entity playerEntity, float deltaTime);


private:
	void _Shoot();

	vector<bool> _children;
};

#endif