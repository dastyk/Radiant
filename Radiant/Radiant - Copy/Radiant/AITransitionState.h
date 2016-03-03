#ifndef _AI_TRANSITION_STATE_H_
#define _AI_TRANSITION_STATE_H_

#include "AIBaseState.h"

class AITransitionState : public AIBaseState
{
public:
	AITransitionState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder);
	~AITransitionState();
	void Enter();
	void Exit();
	void Update(float deltaTime);
	void Init();
	int CheckTransitions();
	int GetType();
	void OnHit(float damage, StatusEffects = STATUS_EFFECT_NORMAL, float duration = 0.0f);
	void GlobalStatus(StatusEffects effect, float duration);
	void SetDamageModifier(float amount);
	void AddToDamageModifier(float amount);
	void OnEnemyDeath();


private:
	bool _reachedGoal = false;
	DirectX::XMFLOAT3 _movementVector;
	DirectX::XMFLOAT3 _currentGoal;
	bool _waiting;
	float _waitTime;
	int _nrOfStepsTaken;
};



#endif // !_AI_TRANSITION_STATE_H_

