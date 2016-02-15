#ifndef  _ENEMY_H_
#define  _ENEMY_H_
#include "Entity.h"
#include "AI_Defines.h"
#include "EntityBuilder.h"

class Enemy
{
private:
	Entity _enemyEntity;
	EntityBuilder* _builder;

	DirectX::XMFLOAT3 _movementVector;
	DirectX::XMFLOAT3 _currentGoal;
	float _speedFactor;
	float _timer = 0.0f;
	bool _waiting = false;
	float _waitTime = 0.0f;
	
	Path* _myPath;
	int _nrOfStepsTaken;

	//The Enemy MUST have an entity assigned to it!
	Enemy();

public:
	Enemy(Entity entity, EntityBuilder* builder);
	~Enemy();

	Entity GetEntity();

	bool UpdateMovement(float deltaTime);
	void Attack(float deltaTime, XMVECTOR playerPosition);
	bool Walking();
	void GivePath(Path* newPath);
	void SetSpeedFactor(float factor);
	DirectX::XMFLOAT3 GetCurrentPos();

};



#endif // ! _ENEMY_H_
