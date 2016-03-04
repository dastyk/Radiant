#ifndef _ENEMY_MINI_GUN_PROJECTILE_H_
#define _ENEMY_MINI_GUN_PROJECTILE_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "General.h"
#include "EntityBuilder.h"
#include "Projectile.h"

class EnemyMiniGunProjectile : public Projectile
{
private:
	XMFLOAT3 _movementVector;

public:
	EnemyMiniGunProjectile(Entity playerEntity, EntityBuilder* builder, XMFLOAT3 parentColor, XMFLOAT3 &playerPosition, float damageModifier);
	virtual ~EnemyMiniGunProjectile();

	void Update(float deltaTime);
	bool GetState();

};

#endif