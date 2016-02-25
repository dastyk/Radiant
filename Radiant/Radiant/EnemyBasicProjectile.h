#ifndef _ENEMY_BASIC_PROJECTILE_H_
#define _ENEMY_BASIC_PROJECTILE_H_

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

class EnemyBasicProjectile : public Projectile
{

private:
	XMFLOAT3 _movementVector;

public:
	EnemyBasicProjectile(Entity playerEntity, EntityBuilder* builder, XMFLOAT3 parentColor, XMFLOAT3 &playerPosition);
	virtual ~EnemyBasicProjectile();

	void Update(float deltaTime);
	bool GetState();

};

#endif