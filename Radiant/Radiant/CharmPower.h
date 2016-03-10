#ifndef _CHARM_POWER_H_
#define _CHARM_POWER_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "Power.h"
#include "List.h"

struct EnemyWithStates;

class CharmPower : public Power
{

public:
	CharmPower(EntityBuilder* builder, Entity player, List<EnemyWithStates>* _Entities);
	virtual ~CharmPower();

	void Update(Entity playerEntity, float deltaTime);
	float Activate(bool& exec, float currentLight);
	bool Upgrade();

private:
	bool _active;
	bool _justFired;
	float _duration;

	List<EnemyWithStates>* _enemies;
	Entity _projectile;



};

#endif