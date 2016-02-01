#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "State.h"
#include "Manager.h"
#include "List.h"
#include "Player.h"
#include "Enemy.h"




class GameState :
	public State
{
private:
	//======================================================================
	//====   Managers for the GameState, keep those entities in check   ====
	//======================================================================
	ManagerWrapper* _managers;


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


	bool _passed;
public:
	GameState();
	~GameState();
	GameState(ManagerWrapper* managers, Player* thePlayer);

	void Init();
	void Shutdown();

	void HandleInput();
	void Update();
	void Render();

	const void DeleteManager();
};




#endif
