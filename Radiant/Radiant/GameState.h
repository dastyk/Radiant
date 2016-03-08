#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "State.h"
#include "Player.h"
#include "Dungeon.h"
#include "Shodan.h"
#include "CPUTimer.h"

class GameState :
	public State
{
private:
	//======================================================================
	//====				Level Specific information						====
	//======================================================================
	float _lightRemaning; //Should be between 0-100
	float _lightTreshold;
	float _timeSinceLastSound;
	int _currentPreQuoteSound;
	int _currentAfterQuoteSound;

	//============================================================
	//====           Used when selecting powers               ====
	//============================================================
	Entity _powerLabel;
	Entity _powerLabelOverlay;
	Entity _choice1;
	Entity _choice1Text;
	Entity _choice2;
	Entity _choice2Text;
	bool _powerChosen;

	//======================================================================
	//====				Player Specific information						====
	//======================================================================
	Player* _player = nullptr;
	Shodan* _AI = nullptr;
	Dungeon* _dungeon = nullptr;
	Entity _map;
	
	CPUTimer _ctimer;
	Entity e4;
	Entity _altar;
	Entity _quadTree;

public:
	GameState();
	~GameState();
	void Init();
	void Shutdown();

	void Update();
	void Render();
};




#endif
