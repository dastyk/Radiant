#ifndef _AI_TELEPORT_MOVE_STATE_H_
#define _AI_TELEPORT_MOVE_STATE_H_


#include "AIBaseState.h"
#include "EnemyDeathBlossomWeapon.h"

class Shodan;

class AITeleportMoveState : public AIBaseState
{
private:
	DirectX::XMFLOAT3 _currentGoal;
	float _timer;
	float _waitTime;

	Path* _myPath = nullptr;
	int _nrOfStepsTaken = 0;
	float _originalIntensity;
	float _arrived = false;

public:
	AITeleportMoveState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder);
	~AITeleportMoveState();
	void Enter();
	void Exit();
	void Update(float deltaTime);
	void Init();
	int CheckTransitions();
	int GetType();


};







#endif
