#ifndef _SHOTGUN_PROJECTILE_H_
#define _SHOTGUN_PROJECTILE_H_

#define NROFSPREAD 7

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
	ShotgunProjectile(Entity playerEntity, EntityBuilder* builder);
	virtual ~ShotgunProjectile();

	void Update(float deltaTime);
	bool GetState();


private:
	ShotgunProjectile(XMVECTOR originPos, XMVECTOR originDir, EntityBuilder* builder);
	ShotgunProjectile* _spread[NROFSPREAD];
};

#endif