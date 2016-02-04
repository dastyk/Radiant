#include "Enemy.h"

Enemy::Enemy()
{

}

Enemy::Enemy(Entity enemyEntity)
{
	_enemyEntity = enemyEntity;
}

Enemy::~Enemy()
{

}

Entity Enemy::GetEntity()
{
	return _enemyEntity;
}

void Enemy::Update()
{
	//Do update code
}