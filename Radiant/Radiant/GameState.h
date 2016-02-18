#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "State.h"
#include "Player.h"
#include "Dungeon.h"
#include "Shodan.h"

class GameState :
	public State
{
private:
	//======================================================================
	//====				Level Specific information						====
	//======================================================================
	float _lightRemaning; //Should be between 0-100
	float _lightTreshold;


	//======================================================================
	//====				Player Specific information						====
	//======================================================================
	Player* _player = nullptr;
	Shodan* _AI = nullptr;
	Dungeon* _dungeon = nullptr;
	Entity _map;
	Entity _altar;

	void _CheckIfLevelDone();
public:
	GameState();
	~GameState();
	void Init();
	void Shutdown();

	void Update();
	void Render();
};




#endif
