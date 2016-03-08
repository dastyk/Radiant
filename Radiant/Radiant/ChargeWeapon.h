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

	void Update(Entity playerEntity, float deltaTime);
	void Shoot();
	bool HasAmmo();
private:
	void _Shoot();

	float _chargeTime;
	bool _chargedLastFrame;
	Entity _chargeEntity;

};

#endif