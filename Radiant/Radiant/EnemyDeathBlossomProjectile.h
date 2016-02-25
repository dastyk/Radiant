#ifndef _ENEMY_DEATH_BLOSSOM_PROJECTILE_H_
#define _ENEMY_DEATH_BLOSSOM_PROJECTILE_H_

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

class EnemyDeathBlossomProjectile : public Projectile
{

private:
	XMFLOAT3 _movementVector;

public:
	EnemyDeathBlossomProjectile(Entity playerEntity, EntityBuilder* builder, XMFLOAT3 parentColor, XMFLOAT3 &playerPosition, float angle);
	virtual ~EnemyDeathBlossomProjectile();

	void Update(float deltaTime);
	bool GetState();

};

#endif