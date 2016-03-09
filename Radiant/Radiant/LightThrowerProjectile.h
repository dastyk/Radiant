#ifndef _LIGHT_THROWER_PROJECTILE_H_
#define _LIGHT_THROWER_PROJECTILE_H_

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

class LightThrowerProjectile : public Projectile
{

public:
	LightThrowerProjectile(Entity playerEntity, EntityBuilder* builder, float damageModifier);
	virtual ~LightThrowerProjectile();

	void Update(float deltaTime);
	bool GetState();
	void CollideWithEntity(DirectX::XMVECTOR& outMTV, const Entity& entity);

private:

};

#endif