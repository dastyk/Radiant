#ifndef  _ENEMY_H_
#define  _ENEMY_H_
#include "Entity.h"
#include "AI_Defines.h"
#include "EntityBuilder.h"
#include "EnemyWeapon.h"

class Enemy
{
private:
	Entity _enemyEntity;
	Entity _rotation;
	EntityBuilder* _builder;

	DirectX::XMFLOAT3 _movementVector;
	DirectX::XMFLOAT3 _currentGoal;
	float _speedFactor;
	float _timer = 0.0f;
	bool _waiting = false;
	float _waitTime = 0.0f;
	
	Path* _myPath;
	int _nrOfStepsTaken;

	float _health = 100.0f;
	float _timeSinceLastSound;

	EnemyWeapon* _weapon = nullptr;
	XMFLOAT3 _myColor;

	//The Enemy MUST have an entity assigned to it!
	Enemy();

public:
	Enemy(Entity entity, EntityBuilder* builder);
	~Enemy();

	Entity GetEntity();
	
	void Update(float deltaTime);
	bool UpdateMovement(float deltaTime);
	void Attack(float deltaTime, XMVECTOR playerPosition);
	bool Walking();
	void GivePath(Path* newPath);
	void SetSpeedFactor(float factor);
	DirectX::XMFLOAT3 GetCurrentPos();
	void SetCurrentGoal(XMFLOAT3 currentGoal);
	DirectX::XMFLOAT3 GetCurrentGoal();
	vector<Projectile*> GetProjectiles();
	void SetCurrentWeapon(EnemyWeapon* myWeapon);
	EnemyWeapon* GetWeapon();
	XMFLOAT3 GetColor();

	float ReduceHealth(float amount);
	float GetHealth();
	float GetTimeSinceLastSound();
	void ResetTimeSinceLastSound();

};



#endif // ! _ENEMY_H_
