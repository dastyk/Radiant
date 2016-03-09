#ifndef _CHARGE_WEAPON_H_
#define _CHARGE_WEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "Weapon.h"
#include "ChargeProjectile.h"

class ChargeWeapon : public Weapon
{

public:
	ChargeWeapon(EntityBuilder* builder, Entity player);
	virtual ~ChargeWeapon();

	void Update(const Entity& playerEntity, float deltaTime);
	bool Shoot(const Entity& playerEntity);
	bool HasAmmo();
private:
	float _chargeTime;
	bool _fire;
	bool _chargedLastFrame;
	Entity _chargeEntity;

};

#endif