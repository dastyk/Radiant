#ifndef _CHARGE_PROJECTILE_H_
#define _CHARGE_PROJECTILE_H_

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

class ChargeProjectile : public Projectile
{

public:
	ChargeProjectile(Entity playerEntity, EntityBuilder* builder, float damageModifier, float radius);
	virtual ~ChargeProjectile();

	void Update(float deltaTime);
	bool GetState();
	void CollideWithEntity(DirectX::XMVECTOR& outMTV, const Entity& entity);

private:
	float _lightRadius;
};

#endif