#ifndef _RANDOM_BLINK_H_
#define _RANDOM_BLINK_H_

#define MAXLIGHTINTENSITY 20

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "Power.h"
#include "Dungeon.h"

class RandomBlink : public Power
{

public:
	RandomBlink(EntityBuilder* builder, Entity player, vector<FreePositions> positions);
	virtual ~RandomBlink();

	void Update(Entity playerEntity, float deltaTime);
	float Activate(bool& exec, float currentLight);
	bool Upgrade();

private:
	vector<FreePositions> viablePositions;
	float _lightIntensity;
	bool _loading;

};

#endif