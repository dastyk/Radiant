#ifndef _AI_PATROL_STATE_H_
#define _AI_PATROL_STATE_H_

#include "AIBaseState.h"

class Shodan;

class AIPatrolState : public AIBaseState
{
private:
	DirectX::XMFLOAT3 _movementVector;
	DirectX::XMFLOAT3 _currentGoal;
	float _speedFactor;
	float _timer;
	bool _waiting;
	float _waitTime;

	Path* _myPath = nullptr;
	int _nrOfStepsTaken = 0;
	bool _playerSeen;
	float _timeUntilPlayerVanishes = 0.0f;

public:
	AIPatrolState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder);
	~AIPatrolState();
	void Enter();
	void Exit();
	void Update(float deltaTime);
	void Init();
	int CheckTransitions();
	int GetType();


};



#endif // !_AI_PATROL_STATE_H_

