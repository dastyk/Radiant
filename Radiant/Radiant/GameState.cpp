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

	State::Init();
	//==================================
	//====		Create Lists		====
	//==================================
	//_enemies->AddElementToList(new Enemy(_managers->CreateObject(
	//	XMVectorSet(0.0f, -15.0f, 0.0f, 0.0f),
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(25.0f, 1.0f, 25.0f, 0.0f),
	//	"Assets/Models/cube.arf",
	//	"Assets/Textures/stonetex.dds",
	//	"Assets/Textures/stonetexnormal.dds")),0);
	//_managers->material->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	//_managers->transform->RotatePitch(_enemies->GetCurrentElement()->GetEntity(), 0);
	//_enemies->AddElementToList(new Enemy(_managers->CreateObject(
	//	XMVectorSet(-10.0f, -10.0f, 10.0f, 0.0f),
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
	//	"Assets/Models/cube.arf",
	//	"Assets/Textures/stonetex.dds",
	//	"Assets/Textures/stonetexnormal.dds")), 1);
	//_managers->material->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	//_managers->light->BindPointLight(_enemies->GetCurrentElement()->GetEntity(), XMFLOAT3(0.0f, 0.0f, 0.0f), 100.0, XMFLOAT3(1.0f, 1.0f, 1.0f), 100.0f);
	//_managers->transform->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);

	//_enemies->AddElementToList(new Enemy(_managers->CreateObject(
	//	XMVectorSet(-20.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(1.0f, 25.0f, 25.0f, 0.0f),
	//	"Assets/Models/cube.arf",
	//	"Assets/Textures/stonetex.dds",
	//	"Assets/Textures/stonetexnormal.dds")), 2);
	//_managers->material->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	//_managers->transform->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);

	//_enemies->AddElementToList(new Enemy(_managers->CreateObject(
	//	XMVectorSet(20.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(1.0f, 25.0f, 25.0f, 0.0f),
	//	"Assets/Models/cube.arf",
	//	"Assets/Textures/stonetex.dds",
	//	"Assets/Textures/stonetexnormal.dds")), 2);
	//_managers->material->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	//_managers->transform->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);

	//_enemies->AddElementToList(new Enemy(_managers->CreateObject(
	//	XMVectorSet(0.0f, 0.0f, -20.0f, 0.0f),
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(25.0f, 25.0f, 1.0f, 0.0f),
	//	"Assets/Models/cube.arf",
	//	"Assets/Textures/stonetex.dds",
	//	"Assets/Textures/stonetexnormal.dds")), 2);
	//_managers->material->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	//_managers->transform->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);

	//_enemies->AddElementToList(new Enemy(_managers->CreateObject(
	//	XMVectorSet(0.0f, 0.0f, 20.0f, 0.0f),
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(25.0f, 25.0f, 1.0f, 0.0f),
	//	"Assets/Models/cube.arf",
	//	"Assets/Textures/stonetex.dds",
	//	"Assets/Textures/stonetexnormal.dds")), 2);
	//_managers->material->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	//_managers->transform->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);



	//==================================
	//====		Set Camera			====
	//==================================
	_player->SetCamera();
	


	map = _managers->entity.Create();
	Dungeon dun(25, 25);
	dun.GetPosVector();
	dun.GetUvVector();
	dun.GetIndicesVector();

	_managers->mesh->CreateStaticMesh(map, "Dungeon", dun.GetPosVector(), dun.GetUvVector(), dun.GetIndicesVector());
	_managers->material->BindMaterial(map, "Shaders/GBuffer.hlsl");
	_managers->material->SetEntityTexture(map, "DiffuseMap", L"Assets/Textures/ft_stone01_c.png");
	_managers->material->SetEntityTexture(map, "NormalMap", L"Assets/Textures/ft_stone01_n.png");
	_managers->material->SetMaterialProperty(map, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_managers->material->SetMaterialProperty(map, 0, "Metalic", 0.1f, "Shaders/GBuffer.hlsl");

	uint i = 15;
	while (i > 0)
	{
		int x = (rand() % (240-10) + 10)/10;
		int y = (rand() % (240 - 10) + 10) / 10;
		if (dun.getTile(x, y) == 0)
		{
			Entity e = _managers->CreateObject(
				XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
				XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
				XMVectorSet(0.1f, 0.1f, 0.1f, 0.0f),
				"Assets/Models/cube.arf",
				"Assets/Textures/ft_stone01_c.png",
				"Assets/Textures/ft_stone01_n.png");

			float r = (rand() % 255) / 255.0f;
			float g = (rand() % 255) / 255.0f;
			float b = (rand() % 255) / 255.0f;
			_managers->light->BindPointLight(e, XMFLOAT3((float)x, 2.0f, y), 1.0f, XMFLOAT3(r, g, b), 5.0);
			_managers->transform->SetPosition(e, XMVectorSet((float)x, 0.5f, (float)y, 0.0f));
			i--;
		}
	}
	
	//_enemies->AddElementToList(new Enemy(_managers->CreateObject(
	//	XMVectorSet(0.0f, 0.0f, 20.0f, 0.0f),
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(25.0f, 25.0f, 1.0f, 0.0f),
	//	"Assets/Models/cube.arf",
	//	"Assets/Textures/stonetex.dds",
	//	"Assets/Textures/stonetexnormal.dds")), 2);
	//_managers->material->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	//_managers->transform->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);




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
	System::GetInput()->HideCursor(true);
 	for (int i = 0; i < _enemies->Size(); i++)
	{
		_enemies->GetCurrentElement()->Update();
		_enemies->MoveCurrent();
	}

	_gameTimer.Tick();
	_test += _gameTimer.DeltaTime();
	/*_managers->light->ChangeLightRange(_enemies->GetElementByID(1)->GetEntity(), 15.0f*abs(sin(_test)));*/

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