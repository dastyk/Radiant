#ifndef _AI_MINI_GUN_LIGHT_STATE_H_
#define _AI_MINI_GUN_LIGHT_STATE_H_

#include "AIBaseState.h"

class AIMiniGunLightState : public AIBaseState
{
public:
	AIMiniGunLightState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder);
	~AIMiniGunLightState();
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
	float _chargeTime = 3.0f;
	float _chargingUp = 0.0f;
	float _timeSinceFireing = 0.0f;
	bool _fireing = false;

};

#endif
