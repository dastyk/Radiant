#ifndef _ROCKET_PROJECTILE_H_
#define _ROCKET_PROJECTILE_H_

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

class RocketProjectile : public Projectile
{

public:
	RocketProjectile(Entity playerEntity, EntityBuilder* builder, float damageModifier);
	virtual ~RocketProjectile();

	void Update(float deltaTime);
	bool GetState();
	void CollideWithEntity(DirectX::XMVECTOR& outMTV, const Entity& entity);

private:
	Entity _tails[3];
};

#endif