#ifndef _LIFE_DRAIN_H_
#define _LIFE_DRAIN_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "Power.h"
#include "List.h"
#include "Player.h"

struct EnemyWithStates;

class LifeDrain : public Power
{

public:
	LifeDrain(EntityBuilder* builder, Entity player, List<EnemyWithStates>* _Entities, Player* thePlayer);
	virtual ~LifeDrain();

	void Update(Entity playerEntity, float deltaTime);
	float Activate(bool& exec, float currentLight);
	bool Upgrade();

private:
	float _duration;
	float _amountDrained;
	List<EnemyWithStates>* _enemies;
	Player* _player;



};

#endif