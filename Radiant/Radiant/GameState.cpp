#include "GameState.h"
#include "System.h"
#include "Graphics.h"

using namespace DirectX;

GameState::GameState() : State()
{
	_managers = nullptr;
	try { _managers = new ManagerWrapper; }
	catch (std::exception& e) { e; throw ErrorMsg(3000002, L"Failed to create managerWrapper in the gamestate."); }
	_passed = false;

	try { _player = new Player(_managers); }
	catch (std::exception& e) { e; throw ErrorMsg(3000005, L"Failed to create a player in the GameState"); }
	

}

GameState::GameState(ManagerWrapper* managers, Player* thePlayer)
{
	//Create a handler for copying previous level and manager, while still removing the old data
	//About entites, only keeping the meshes (less load time deluxe)
}

GameState::~GameState()
{

}

void GameState::Init()
{
	//==================================
	//====		Create Managers		====
	//==================================

	

	//==================================
	//====		Create Lists		====
	//==================================


	//==================================
	//====		Set Input data		====
	//==================================
	System::GetInput()->LockMouseToCenter(true);
	System::GetInput()->LockMouseToWindow(true);
	System::GetInput()->HideCursor(true);
	
	
}

void GameState::Shutdown()
{
	State::Shutdown();
	if (!_passed)
	{
		delete _player;
		DeleteManager();
	}
	
	System::GetInput()->LockMouseToCenter(false);
	System::GetInput()->LockMouseToWindow(false);
	System::GetInput()->HideCursor(false);
	
}

void GameState::HandleInput()
{
	if (System::GetInput()->IsKeyDown(VK_ESCAPE))
		throw FinishMsg(1);
}

void GameState::Update()
{
	_gameTimer.Tick();
}

void GameState::Render()
{
	//Fucken render!
}

const void GameState::DeleteManager()
{
	SAFE_DELETE(_managers);
}