#ifndef _LOCK_ON_H_
#define _LOCK_ON_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "Power.h"
#include "Shodan.h"

class LockOnStrike : public Power
{

public:
	LockOnStrike(EntityBuilder* builder, Entity player, List<EnemyWithStates>* _Entities);
	virtual ~LockOnStrike();

	void Update(Entity playerEntity, float deltaTime);

private:
	void _MoveProjectiles(Entity playerEntity, float deltaTime);

	List<EnemyWithStates>* _enemies;

	vector<Entity> _projectiles;
	vector<float> _angles;
	vector<bool> _foundTarget;
	vector<XMFLOAT3> _directions;



};

#endif