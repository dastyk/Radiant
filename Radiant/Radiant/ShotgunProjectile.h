#ifndef _SHOTGUN_PROJECTILE_H_
#define _SHOTGUN_PROJECTILE_H_

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

class ShotgunProjectile : public Projectile
{

public:
	ShotgunProjectile(Entity playerEntity, EntityBuilder* builder, float damageModifier);
	virtual ~ShotgunProjectile();

	void Update(float deltaTime);
	bool GetState();

};

#endif