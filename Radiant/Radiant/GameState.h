#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "State.h"
#include "Player.h"
#include "Dungeon.h"
#include "Shodan.h"
#include "CPUTimer.h"

enum Difficulty
{
	EASY_DIFFICULTY = 1,
	NORMAL_DIFFICULTY = 2,
	HARD_DIFFICULTY = 3,
	WHY_DID_YOU_CHOOSE_THIS_DIFFICULTY = 4
};

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
	int _currentLevel;

	//============================================================
	//====           Used when selecting powers               ====
	//============================================================
	std::vector<Power*> _allPowers;
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
	GameState(Player* player, int lastLevel);
	
	~GameState();
	void Init();
	void Shutdown();

	void Update();
	void Render();
};




#endif
