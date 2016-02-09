#ifndef _PROJECTILE_H_
#define _PROJECTILE_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "System.h"

class Projectile
{

public:
	virtual void Update() = 0;


private:
	float _lifeTime;
	float _damage;

	Entity _projectileEntity;

};

#endif