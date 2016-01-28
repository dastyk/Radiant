#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "State.h"
#include "Manager.h"
#include "List.h"




class GameState :
	public State
{
private:
	//======================================================================
	//====   Managers for the GameState, keep those entities in check   ====
	//======================================================================
	EntityManager* _entityManager;
	StaticMeshManager* _staticMeshManager;



	//======================================================================
	//====					Entities to keep track of.					====
	//======================================================================
	
	//List<Enemies>* _Enemies;	<--- Replace with "correct" type
	
	//List<Interior>* _interior; <--- Replace with "correct" type

	//List<Projectile>* _projectiles;

	//======================================================================
	//====				Level Specific information						====
	//======================================================================
	float _LightRemaning; //Should be between 0-100
	float _LightTreshold;

	
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
