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
protected:
	Projectile(EntityBuilder* builder, Entity owner, float damageModifier) : _builder(builder), _owner(owner), _damageModifier(damageModifier),_lifeTime(0),_damage(0),_alive(false),_projectileEntity(Entity()) {}
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
		return _damage*_damageModifier;
	}

	virtual void SetState(bool value)
	{
		_alive = value;
	}

	virtual Entity GetOwner()
	{
		return _owner;
	}


	virtual void CollideWithEntity(DirectX::XMVECTOR& outMTV, const Entity& entity) { _alive = false; }
protected:
	float _lifeTime;
	float _damage;
	float _damageModifier;
	bool _alive;
	Entity _owner;

	Entity _projectileEntity;
	EntityBuilder* _builder;
};

#endif