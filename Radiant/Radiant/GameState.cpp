#include "GameState.h"
#include "System.h"
#include "Graphics.h"

using namespace DirectX;

GameState::GameState() : State()
{
	_managers = nullptr;
	try { _managers = new ManagerWrapper; }
	catch (std::exception& e) { e; throw ErrorMsg(3000002, L"Failed to create managerWrapper in the GameState."); }
	_passed = false;

	try { _player = new Player(_managers); }
	catch (std::exception& e) { e; throw ErrorMsg(3000005, L"Failed to create a player in the GameState."); }
	
	try { _enemies = new List<Enemy>; }
	catch (std::exception& e) { e; throw ErrorMsg(3000006, L"Failed to allocate memory for the enemy list in the GameState."); }
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
	for (int i = 0; i < 10; i++)
	{
		_enemies->AddElementToList(new Enemy(_managers->CreateObject(
			XMVectorSet(i, i, i*i%2, 0.0f),
			XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
			"Assets/Models/cube.arf",
			"Assets/Textures/stonetex.dds",
			"Assets/Textures/stonetexnormal.dds")),i);

		_managers->material->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	}

	//==================================
	//====		Set Input data		====
	//==================================
	System::GetInput()->LockMouseToCenter(true);
	System::GetInput()->LockMouseToWindow(true);
	System::GetInput()->HideCursor(true);


	//==================================
	//====		Set Camera			====
	//==================================
	_player->SetCamera();
	
}

void GameState::Shutdown()
{
	State::Shutdown();
	if (!_passed)
	{
		delete _enemies;
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

	_player->HandleInput(_gameTimer.DeltaTime());
}

void GameState::Update()
{
	for (int i = 0; i < 10; i++)
	{
		_managers->transform->RotatePitch(_enemies->GetCurrentElement()->GetEntity(), 1);
		_enemies->MoveCurrent();
	}	
	_gameTimer.Tick();
	_player->Update(_gameTimer.DeltaTime());
}

void GameState::Render()
{
	System::GetGraphics()->Render(_gameTimer.TotalTime(), _gameTimer.DeltaTime());
}

const void GameState::DeleteManager()
{
	SAFE_DELETE(_managers);
}