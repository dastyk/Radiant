#ifndef _AI_STATE_CONTROLLER_H_
#define _AI_STATE_CONTROLLER_H_

#include "AIStates.h"
#include "Utils.h"
#include "StatusEffects.h"

class AIStateController
{
private:
	vector<AIBaseState*> _AIStates;
	AIBaseState* _currentState;
	AIBaseState* _defaultState;

	int _type;
	float _timeSinceTransistion;
	float _thinkingTime;
	bool _allowedToChangeState;

	//Zee states I've no idea what the fuck they're all about.
	AIBaseState* _goalState;
	int _goalID;

public:
	AIStateController(int type = AI_STATE_NONE);
	virtual ~AIStateController();

	virtual void UpdateMachine(float deltaTime);
	virtual void OnHit(float damageDone, StatusEffects effect = STATUS_EFFECT_NORMAL, float durationOfEffect = 0.0f);
	virtual void GlobalStatus(StatusEffects effect, float duration);
	virtual void AddState(AIBaseState* state);
	virtual void SetDefaultState(AIBaseState* state);
	virtual void SetGoalID(int goal);
	virtual bool TransitionState(int goal);
	virtual void Reset();
	virtual void SetThinkingTime(float time);
	virtual void AddToDamageModifier(float amount);
	virtual void SetDamageModifier(float amount);
	virtual void OnEnemyDeath();
};





#endif

