#ifndef _REGENPOWER_H_
#define _REGENPOWER_H_

#pragma once
#include "Power.h"
#include "Player.h"
class RegenPower :
	public Power
{
public:
	RegenPower(EntityBuilder* builder, Player* thePlayer, Entity playerEnt);
	~RegenPower();


	void Update(Entity playerEntity, float deltaTime);
	float Activate(bool& exec, float currentLight);
	bool Upgrade();

private:
	Player* _player;
	int _status;
	float _regenPercent;
	float _regenTime;
};

#endif