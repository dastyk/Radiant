#ifndef _TIME_STOPPER_H_
#define _TIME_STOPPER_H_

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

class TimeStopper : public Power
{

public:
	TimeStopper(EntityBuilder* builder, Entity player, List<EnemyWithStates>* _Entities);
	virtual ~TimeStopper();

	void Update(Entity playerEntity, float deltaTime);
	float Activate(bool& exec, float currentLight);
	bool Upgrade();

private:
	float _duration;
	List<EnemyWithStates>* _enemies;



};

#endif