#include "GameState.h"
#include "System.h"
#include "Graphics.h"

using namespace DirectX;

GameState::GameState() : State()
{

	//==================================
	//====	Create All Things		====
	//==================================

	_managers = nullptr;
	try { _managers = new ManagerWrapper; }
	catch (std::exception& e) { e; throw ErrorMsg(3000002, L"Failed to create managerWrapper in the GameState."); }
	_passed = false;
	_managers->SetExclusiveRenderAccess();
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
	//====		Create Lists		====
	//==================================
	_enemies->AddElementToList(new Enemy(_managers->CreateObject(
		XMVectorSet(0.0f, -15.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(25.0f, 1.0f, 25.0f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/stonetex.dds",
		"Assets/Textures/stonetexnormal.dds")),0);
	_managers->material->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_managers->transform->RotatePitch(_enemies->GetCurrentElement()->GetEntity(), 0);
	_enemies->AddElementToList(new Enemy(_managers->CreateObject(
		XMVectorSet(-10.0f, -10.0f, 10.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/stonetex.dds",
		"Assets/Textures/stonetexnormal.dds")), 1);
	_managers->material->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_managers->light->BindPointLight(_enemies->GetCurrentElement()->GetEntity(), XMFLOAT3(0.0f, 0.0f, 0.0f), 100.0, XMFLOAT3(1.0f, 1.0f, 1.0f), 100.0f);
	_managers->transform->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);

	_enemies->AddElementToList(new Enemy(_managers->CreateObject(
		XMVectorSet(-20.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(1.0f, 25.0f, 25.0f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/stonetex.dds",
		"Assets/Textures/stonetexnormal.dds")), 2);
	_managers->material->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_managers->transform->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);

	_enemies->AddElementToList(new Enemy(_managers->CreateObject(
		XMVectorSet(20.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(1.0f, 25.0f, 25.0f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/stonetex.dds",
		"Assets/Textures/stonetexnormal.dds")), 2);
	_managers->material->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_managers->transform->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);

	_enemies->AddElementToList(new Enemy(_managers->CreateObject(
		XMVectorSet(0.0f, 0.0f, -20.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(25.0f, 25.0f, 1.0f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/stonetex.dds",
		"Assets/Textures/stonetexnormal.dds")), 2);
	_managers->material->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_managers->transform->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);

	_enemies->AddElementToList(new Enemy(_managers->CreateObject(
		XMVectorSet(0.0f, 0.0f, 20.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(25.0f, 25.0f, 1.0f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/stonetex.dds",
		"Assets/Textures/stonetexnormal.dds")), 2);
	_managers->material->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_managers->transform->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);

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
	
	State::Init();
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
	if (System::GetInput()->GetKeyStateAndReset(VK_F1))
		throw StateChange(new MenuState);
	if (System::GetInput()->GetKeyStateAndReset(VK_ESCAPE))
		throw FinishMsg(1);

	_player->HandleInput(_gameTimer.DeltaTime());
}

void GameState::Update()
{
 	for (int i = 0; i < _enemies->Size(); i++)
	{
		_enemies->GetCurrentElement()->Update();
		_enemies->MoveCurrent();
	}

	_gameTimer.Tick();
	_test += _gameTimer.DeltaTime();
	_managers->light->ChangeLightRange(_enemies->GetElementByID(1)->GetEntity(), 15.0f*abs(sin(_test)));

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