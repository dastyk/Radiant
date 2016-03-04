#ifndef _AI_WALK_INTO_THE_LIGHT_ATTACK_STATE_H_
#define _AI_WALK_INTO_THE_LIGHT_ATTACK_STATE_H_

#include "AIBaseState.h"

class AIWalkIntoTheLightAttackState : public AIBaseState
{
public:
	AIWalkIntoTheLightAttackState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder);
	~AIWalkIntoTheLightAttackState();
	void Enter();
	void Exit();
	void Update(float deltaTime);
	void Init();
	int CheckTransitions();
	int GetType();
	void OnHit(float damage, StatusEffects = STATUS_EFFECT_NORMAL, float duration = 0.0f);
	void GlobalStatus(StatusEffects effect, float duration);
	void AddToDamageModifier(float amount);
	void SetDamageModifier(float amount);
	void OnEnemyDeath();


private:
	XMFLOAT3 _movementVector;
	float _myIntensity;
	float _timeSinceUpdate;
	float _timeBeforeUpdate;

};

#endif
