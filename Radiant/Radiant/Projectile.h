#ifndef _PROJECTILE_H_
#define _PROJECTILE_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
//#include "System.h"
#include "General.h"
#include "EntityBuilder.h"

class Projectile
{

public:
	virtual void Update(float deltaTime) = 0;

	virtual bool GetState() = 0;

protected:
	float _lifeTime;
	float _damage;
	bool _alive;

	Entity _projectileEntity;

};

#endif