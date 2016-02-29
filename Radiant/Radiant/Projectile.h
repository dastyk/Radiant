#ifndef _PROJECTILE_H_
#define _PROJECTILE_H_

#define ENEMY_OWNER 0
#define PLAYER_OWNER 1

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
protected:
	Projectile(EntityBuilder* builder) : _builder(builder) {}
public:
	virtual void Update(float deltaTime) = 0;
	virtual ~Projectile() { _builder->GetEntityController()->ReleaseEntity(_projectileEntity); }

	virtual bool GetState() = 0;

	virtual Entity GetEntity()
	{
		return _projectileEntity;
	}

	virtual float GetDamage()
	{
		return _damage;
	}

	virtual void SetState(bool value)
	{
		_alive = value;
	}
	virtual void CollideWithEntity(DirectX::XMVECTOR& outMTV, const Entity& entity) { _alive = false; }
protected:
	float _lifeTime;
	float _damage;
	bool _alive;
	int _owner; 

	Entity _projectileEntity;
	EntityBuilder* _builder;
};

#endif