#include "GameState.h"

GameState::GameState()
{
	_entityManager = nullptr;
	_staticMeshManager = nullptr;

}

GameState::~GameState()
{
	this->Shutdown();
}

void GameState::Init()
{
	//==================================
	//====		Create Managers		====
	//==================================

	//_entityManager = new EntityManager();
	//if (!_entityManager)
	{
		//throw
	}

	//_staticMeshManager = new StaticMeshManager();
	//if (!_staticMeshManager)
	{
		//throw
	}

	//==================================
	//====		Create Lists		====
	//==================================

	
}

void GameState::Shutdown()
{
	if (_entityManager)
	{
		delete _entityManager;
		_entityManager = nullptr;
	}

	if (_staticMeshManager)
	{
		delete _staticMeshManager;
		_staticMeshManager = nullptr;
	}
}

void GameState::HandleInput()
{
	//Focken do stuff!
}

void GameState::Update()
{
	//Fucken do the update!
}

void GameState::Render()
{
	//Fucken render!
}