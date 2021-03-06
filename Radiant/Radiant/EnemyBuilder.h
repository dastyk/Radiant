#ifndef _ENEMY_BUILDER_H_
#define _ENEMY_BUILDER_H_

#include "EntityBuilder.h"
#include "Enemy.h"
#include "AIStateController.h"

#define NROFENEMYTYPES 3
#define STARTINTENSITYLIGHT 3
#define STARTBLOBRANGELIGHT 1.0f
#define STARTRANGELIGHT 0.5f

enum class EnemyTypes : unsigned
{
	ENEMY_TYPE_NORMAL = 1 << 0,
	ENEMY_TYPE_TELEPORTER = 1 << 1,
	ENEMY_TYPE_MINI_GUN = 1 << 2,
	ENEMY_TYPE_SHADOW = 1 << 3,
	ENEMY_TYPE_PROXIMITY_SITH = 1 << 4
};
struct EnemyWithStates
{
	Enemy* _thisEnemy;
	AIStateController* _thisEnemyStateController;
    virtual	~EnemyWithStates() { delete _thisEnemy; delete _thisEnemyStateController; };
};

class EnemyBuilder
{
private:
	EntityBuilder* _builder;
	Shodan* _controller;
	EnemyBuilder();
	
public:

	EnemyBuilder(EntityBuilder* builder, Shodan* controller) { _builder = builder; _controller = controller; };
	~EnemyBuilder();
	EnemyWithStates* AddNewEnemy(const XMFLOAT3 &position);
	EnemyWithStates* AddNewEnemy(const XMFLOAT3 &position, const EnemyTypes typeOfEnemy);


};









#endif
