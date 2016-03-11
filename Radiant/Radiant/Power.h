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
enum power_id_t { LOCK_ON_STRIKE, RANDOMBLINK, CHARMPOWER };
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

	virtual std::string GetDescription(int textWidth = 40) const;
	virtual power_id_t GetType() const;
	virtual int GetPowerLevel() const;
	

	//virtual void Shoot() = 0;
protected:
	std::string _powerName;
	std::string _description;
	float _cooldown;
	float _timeSinceLastActivation;
	int _powerLevel;
	power_id_t _type;
	Entity _powerEntity;
	EntityBuilder* _builder;
};

#endif