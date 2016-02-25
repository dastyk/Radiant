#ifndef _ENEMY_DEATH_BLOSSOM_WEAPON_H_
#define _ENEMY_DEATH_BLOSSOM_WEAPON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "EnemyWeapon.h"
#include "EnemyDeathBlossomProjectile.h"

class EnemyDeathBlossomWeapon : public EnemyWeapon
{

public:
	EnemyDeathBlossomWeapon(EntityBuilder* builder, XMFLOAT3 color);
	virtual ~EnemyDeathBlossomWeapon();

	bool Update(Entity playerEntity, float deltaTime, XMFLOAT3 &playerPosition);
	void Shoot();
	void Reset();
	void SetCooldownTime(float cooldown);

private:


};

#endif