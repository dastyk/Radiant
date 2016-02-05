#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "State.h"
#include "List.h"
#include "Player.h"
#include "Enemy.h"
#include "Dungeon.h"




class GameState :
	public State
{
private:
	//======================================================================
	//====					Entities to keep track of.					====
	//======================================================================
	
	List<Enemy>* _enemies;	//<--- Replace with "correct" type
	
	//List<Interior>* _interior; <--- Replace with "correct" type

	//List<Projectile>* _projectiles;

	//======================================================================
	//====				Level Specific information						====
	//======================================================================
	float _lightRemaning; //Should be between 0-100
	float _lightTreshold;


	//======================================================================
	//====				Player Specific information						====
	//======================================================================
	Player* _player;

	float _test = 0;
	Entity map;
public:
	GameState();
	~GameState();
	void Init();
	void Shutdown();

	void HandleInput();
	void Update();
	void Render();
};




#endif
