#ifndef _REGENPOWER_H_
#define _REGENPOWER_H_

#pragma once
#include "Power.h"
class RegenPower :
	public Power
{
public:
	RegenPower();
	~RegenPower();


	void Update(Entity playerEntity, float deltaTime);
	float Activate(bool& exec, float currentLight);
	bool Upgrade();
};

#endif