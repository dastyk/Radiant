#ifndef _POWER_H_
#define _POWER_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "EntityBuilder.h"

class Power
{
protected:
	Power(EntityBuilder* builder) : _builder(builder) {}
public:
	virtual void Update(Entity playerEntity, float deltaTime) = 0;
	virtual ~Power()
	{

	}

	virtual void setActive(bool value)
	{
		_active = value;
	}

	virtual bool Upgrade() = 0;

	//virtual void Shoot() = 0;
protected:
	float _cooldown;
	float _timeSinceLastActivation;
	bool _active;
	int _powerLevel;

	Entity _powerEntity;
	EntityBuilder* _builder;
};

#endif