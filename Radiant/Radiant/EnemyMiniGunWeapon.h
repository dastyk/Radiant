#ifndef _ENEMY_MINI_GUN_WEAPON_H_
#define _ENEMY_MINI_GUN_WEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "EnemyWeapon.h"
#include "EnemyMiniGunProjectile.h"

class EnemyMiniGunWeapon : public EnemyWeapon
{

public:
	EnemyMiniGunWeapon(EntityBuilder* builder, XMFLOAT3 color);
	virtual ~EnemyMiniGunWeapon();

	bool Update(Entity thisEntity, float deltaTime, XMFLOAT3 &playerPosition, float damageMultiplier);
	bool Shoot();
	void Reset();
	void SetCooldownTime(float cooldown);

private:
	float _angle;


};

#endif
