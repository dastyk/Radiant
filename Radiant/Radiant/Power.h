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
	Power(EntityBuilder* builder) : _builder(builder),_cooldown(0),_timeSinceLastActivation(0.0f), _powerLevel(0),_powerEntity(Entity()) {}
public:
	virtual void Update(Entity playerEntity, float deltaTime) = 0;
	virtual ~Power()
	{

	}

	virtual float Activate(bool& exec, float currentLight) = 0;

	virtual bool Upgrade() = 0;

	//virtual void Shoot() = 0;
protected:
	float _cooldown;
	float _timeSinceLastActivation;
	int _powerLevel;

	Entity _powerEntity;
	EntityBuilder* _builder;
};

#endif