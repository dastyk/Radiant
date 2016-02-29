#ifndef _BOUNCE_PROJECTILE_H_
#define _BOUNCE_PROJECTILE_H_

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

class BounceProjectile : public Projectile
{

public:
	BounceProjectile(Entity playerEntity, EntityBuilder* builder);
	virtual ~BounceProjectile();

	void Update(float deltaTime);
	bool GetState();

};

#endif