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
	FragBombWeapon(EntityBuilder* builder, Entity player);
	virtual ~FragBombWeapon();

	void Update(const Entity& playerEntity, float deltaTime);
	bool Shoot(const Entity& playerEntity);
private:

	vector<bool> _children;
};

#endif