#ifndef  _AI_BASE_STATE_H_
#define _AI_BASE_STATE_H_

#include "Enemy.h"
#include "StatusEffects.h"

class Shodan;

enum AIStates
{
	AI_STATE_NONE = 0,
	AI_STATE_PATROL = 1,
	AI_STATE_ATTACK = 2,
	AI_STATE_RUN_AWAY = 3,
	AI_STATE_TRANSITION = 4
};

class AIBaseState
{
public:
	AIBaseState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder) { _currentState = _currentState; _controller = controller; _myEnemy = myEnemy; _builder = builder; _resetIntensity = _builder->Light()->GetLightIntensity(_myEnemy->GetEntity()); };
	virtual ~AIBaseState();
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void Update(float deltaTime);
	virtual void Init() = 0;
	virtual int CheckTransitions() = 0;
	virtual int GetType() = 0;
	virtual void OnHit(float damage, StatusEffects = STATUS_EFFECT_NORMAL, float duration = 0.0f) = 0;
	virtual void GlobalStatus(StatusEffects effect, float duration) = 0;
	virtual void AddToDamageModifier(float amount) = 0;
	virtual void SetDamageModifier(float amount) = 0;
	virtual void OnEnemyDeath() = 0;
	void ThisOneDied();



protected:
	Shodan* _controller;
	EntityBuilder* _builder;
	int _currentState;
	Enemy* _myEnemy;
	bool _beenHit = false;
	float  _glowOnHitTimer = 0.0f;
	float _resetIntensity;

};





#endif // ! _BASE_AI_STATE_H_

