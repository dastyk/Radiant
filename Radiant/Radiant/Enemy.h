#ifndef  _ENEMY_H_
#define  _ENEMY_H_
#include "General.h"
#include "Entity.h"

class Enemy
{
private:
	Entity _enemyEntity;

	//The Enemy MUST have an entity assigned to it!
	Enemy();

public:
	Enemy(Entity enemyEntity);
	~Enemy();

};



#endif // ! _ENEMY_H_
