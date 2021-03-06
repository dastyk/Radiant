#ifndef _BASIC_PROJECTILE_H_
#define _BASIC_PROJECTILE_H_

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

class BasicProjectile : public Projectile
{

public:
	BasicProjectile(Entity playerEntity, EntityBuilder* builder, float damageModifier);
	virtual ~BasicProjectile();

	void Update(float deltaTime);
	bool GetState();
	void CollideWithEntity(DirectX::XMVECTOR& outMTV, const Entity& entity);
};

#endif