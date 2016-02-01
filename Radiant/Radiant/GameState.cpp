#include "GameState.h"
#include "System.h"
#include "Graphics.h"

using namespace DirectX;

GameState::GameState() : State()
{
	_managers = nullptr;
	try { _managers = new ManagerWrapper; }
	catch (std::exception& e) { e; throw ErrorMsg(1200002, L"Failed to create managerwrapper."); }
	_passed = false;
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