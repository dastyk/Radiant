#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "State.h"
#include "Player.h"
#include "Dungeon.h"
#include "Shodan.h"
#include "CPUTimer.h"

enum class Difficulty : unsigned
{
	EASY_DIFFICULTY = 1 << 0,
	NORMAL_DIFFICULTY = 1 << 1,
	HARD_DIFFICULTY = 1 << 2,
	WHY_DID_YOU_CHOOSE_THIS_DIFFICULTY = 1 << 3
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
	uint _currentLevel;

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
	

	//======================================================================
	//====				Player Specific information						====
	//======================================================================
	Player* _player = nullptr;
	Shodan* _AI = nullptr;
	Dungeon* _dungeon = nullptr;
	Entity* _clutter = nullptr;
	
	CPUTimer _ctimer;
	Entity e4;
	Entity _altar;
	Entity _quadTree;
	Entity _altarCenterLight;
	static const int _numAltarBolts = 3;
	Entity _altarBolts[_numAltarBolts];
	float _altarBoltAngle[_numAltarBolts];

	std::vector<Entity> _weaponSpawns;
public:
	GameState();
	GameState(Player* player, int lastLevel);
	GameState(Player* player, power_id_t powerToGive);
	
	~GameState();
	void Init();
	void Shutdown();

	void Update();
	void Render();

	void ProgressNoNextLevel(unsigned int power);
	Player* GetPlayer();
private:
	void _CreateWeapons(unsigned int types, unsigned int nrofweps);
};




#endif
