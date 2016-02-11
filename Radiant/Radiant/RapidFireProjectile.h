#ifndef _RAPID_FIRE_PROJECTILE_H_
#define _RAPID_FIRE_PROJECTILE_H_

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

class RapidFireProjectile : public Projectile
{

public:
	RapidFireProjectile(Entity playerEntity, EntityBuilder* builder);
	virtual ~RapidFireProjectile();

	void Update(float deltaTime);
	bool GetState();


private:
	EntityBuilder* _builder;
};

#endif