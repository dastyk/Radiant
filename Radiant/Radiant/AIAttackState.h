#ifndef _AI_ATTACK_STATE_H_
#define _AI_ATTACK_STATE_H_

#include "AIBaseState.h"

class AIAttackState : public AIBaseState
{
public:
	AIAttackState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder);
	virtual ~AIAttackState();
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
	XMFLOAT3 _lastAttackVector;

};

#endif
