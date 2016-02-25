#ifndef  _AI_BASE_STATE_H_
#define _AI_BASE_STATE_H_

#include "Enemy.h"


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
	AIBaseState(int currentState, Shodan* controller, Enemy* myEnemy, EntityBuilder* builder) { _currentState = _currentState; _controller = controller; _myEnemy = myEnemy; _builder = builder; };
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void Update(float deltaTime);
	virtual void Init() = 0;
	virtual int CheckTransitions() = 0;
	virtual int GetType() = 0;



protected:
	Shodan* _controller;
	EntityBuilder* _builder;
	int _currentState;
	Enemy* _myEnemy;

};





#endif // ! _BASE_AI_STATE_H_

