#ifndef _ENEMY_BASIC_WEAPON_H_
#define _ENEMY_BASIC_WEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "EnemyWeapon.h"
#include "EnemyBasicProjectile.h"

class EnemyBasicWeapon : public EnemyWeapon
{

public:
	EnemyBasicWeapon(EntityBuilder* builder, XMFLOAT3 color);
	virtual ~EnemyBasicWeapon();

	bool Update(Entity thisEntity, float deltaTime, XMFLOAT3 &playerPosition, float damageMultiplier);
	void Shoot();
	void Reset();
	void SetCooldownTime(float cooldown);

private:


};

#endif
