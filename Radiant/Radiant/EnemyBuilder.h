#ifndef _ENEMY_BUILDER_H_
#define _ENEMY_BUILDER_H_

#include "EntityBuilder.h"
#include "Enemy.h"
#include "AIStateController.h"

#define NROFENEMYTYPES 2
#define STARTINTENSITYLIGHT 3
#define STARTBLOBRANGELIGHT 1.0f
#define STARTRANGELIGHT 0.5f

enum EnemyTypes
{
	ENEMY_TYPE_NORMAL = 0,
	ENEMY_TYPE_TELEPORTER = 1

};

struct EnemyWithStates
{
	Enemy* _thisEnemy;
	AIStateController* _thisEnemyStateController;
	~EnemyWithStates() { delete _thisEnemy; delete _thisEnemyStateController; };
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
