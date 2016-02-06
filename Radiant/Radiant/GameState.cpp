#include "GameState.h"
#include "System.h"
#include "Graphics.h"

using namespace DirectX;

GameState::GameState() : State()
{



	
}

GameState::~GameState()
{

}

void GameState::Init()
{

	//==================================
	//====	Create All Things		====
	//==================================
	try { _player = new Player(_builder); }
	catch (std::exception& e) { e; throw ErrorMsg(3000005, L"Failed to create a player in the GameState."); }

	try { _enemies = new List<Enemy>; }
	catch (std::exception& e) { e; throw ErrorMsg(3000006, L"Failed to allocate memory for the enemy list in the GameState."); }

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
	//_builder->Material()->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	//_builder->Transform()->RotatePitch(_enemies->GetCurrentElement()->GetEntity(), 0);
	//_enemies->AddElementToList(new Enemy(_managers->CreateObject(
	//	XMVectorSet(-10.0f, -10.0f, 10.0f, 0.0f),
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
	//	"Assets/Models/cube.arf",
	//	"Assets/Textures/stonetex.dds",
	//	"Assets/Textures/stonetexnormal.dds")), 1);
	//_builder->Material()->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	//_builder->Light->BindPointLight(_enemies->GetCurrentElement()->GetEntity(), XMFLOAT3(0.0f, 0.0f, 0.0f), 100.0, XMFLOAT3(1.0f, 1.0f, 1.0f), 100.0f);
	//_builder->Transform()->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);

	//_enemies->AddElementToList(new Enemy(_managers->CreateObject(
	//	XMVectorSet(-20.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(1.0f, 25.0f, 25.0f, 0.0f),
	//	"Assets/Models/cube.arf",
	//	"Assets/Textures/stonetex.dds",
	//	"Assets/Textures/stonetexnormal.dds")), 2);
	//_builder->Material()->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	//_builder->Transform()->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);

	//_enemies->AddElementToList(new Enemy(_managers->CreateObject(
	//	XMVectorSet(20.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(1.0f, 25.0f, 25.0f, 0.0f),
	//	"Assets/Models/cube.arf",
	//	"Assets/Textures/stonetex.dds",
	//	"Assets/Textures/stonetexnormal.dds")), 2);
	//_builder->Material()->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	//_builder->Transform()->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);

	//_enemies->AddElementToList(new Enemy(_managers->CreateObject(
	//	XMVectorSet(0.0f, 0.0f, -20.0f, 0.0f),
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(25.0f, 25.0f, 1.0f, 0.0f),
	//	"Assets/Models/cube.arf",
	//	"Assets/Textures/stonetex.dds",
	//	"Assets/Textures/stonetexnormal.dds")), 2);
	//_builder->Material()->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	//_builder->Transform()->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);

	//_enemies->AddElementToList(new Enemy(_managers->CreateObject(
	//	XMVectorSet(0.0f, 0.0f, 20.0f, 0.0f),
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	//	XMVectorSet(25.0f, 25.0f, 1.0f, 0.0f),
	//	"Assets/Models/cube.arf",
	//	"Assets/Textures/stonetex.dds",
	//	"Assets/Textures/stonetexnormal.dds")), 2);
	//_builder->Material()->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	//_builder->Transform()->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);



	//==================================
	//====		Set Camera			====
	//==================================
	_player->SetCamera();
	


	map = _builder->EntityC().Create();
	Dungeon dun(25, 25);
	dun.GetPosVector();
	dun.GetUvVector();
	dun.GetIndicesVector();

	_builder->Mesh()->CreateStaticMesh(map, "Dungeon", dun.GetPosVector(), dun.GetUvVector(), dun.GetIndicesVector());
	_builder->Material()->BindMaterial(map, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetEntityTexture(map, "DiffuseMap", L"Assets/Textures/ft_stone01_c.png");
	_builder->Material()->SetEntityTexture(map, "NormalMap", L"Assets/Textures/ft_stone01_n.png");
	_builder->Material()->SetMaterialProperty(map, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(map, 0, "Metalic", 0.1f, "Shaders/GBuffer.hlsl");

	uint i = 15;
	while (i > 0)
	{
		int x = (rand() % (240-10) + 10)/10;
		int y = (rand() % (240 - 10) + 10) / 10;
		if (dun.getTile(x, y) == 0)
		{
			Entity e = _builder->CreateObject(
				XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
				XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
				XMVectorSet(0.1f, 0.1f, 0.1f, 0.0f),
				"Assets/Models/cube.arf",
				"Assets/Textures/ft_stone01_c.png",
				"Assets/Textures/ft_stone01_n.png");

			float r = (rand() % 255) / 255.0f;
			float g = (rand() % 255) / 255.0f;
			float b = (rand() % 255) / 255.0f;
			_builder->Light()->BindPointLight(e, XMFLOAT3((float)x, 2.0f, y), 1.0f, XMFLOAT3(r, g, b), 5.0);
			_builder->Transform()->SetPosition(e, XMVectorSet((float)x, 0.5f, (float)y, 0.0f));
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
	//_builder->Material()->SetMaterialProperty(_enemies->GetCurrentElement()->GetEntity(), 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	//_builder->Transform()->RotateRoll(_enemies->GetCurrentElement()->GetEntity(), 0.0f);




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
		
	}
	
	System::GetInput()->LockMouseToCenter(false);
	System::GetInput()->LockMouseToWindow(false);
	System::GetInput()->HideCursor(false);
	
}

void GameState::HandleInput()
{
	timer.TimeStart("Input");
	if (System::GetInput()->GetKeyStateAndReset(VK_ESCAPE))
		throw StateChange(new MenuState);

	_player->HandleInput(_gameTimer.DeltaTime());
	timer.TimeEnd("Input");
}

void GameState::Update()
{
	timer.TimeStart("Update");

 	for (int i = 0; i < _enemies->Size(); i++)
	{
		_enemies->GetCurrentElement()->Update();
		_enemies->MoveCurrent();
	}

	_gameTimer.Tick();
	_test += _gameTimer.DeltaTime();
	/*_builder->Light->ChangeLightRange(_enemies->GetElementByID(1)->GetEntity(), 15.0f*abs(sin(_test)));*/

	_player->Update(_gameTimer.DeltaTime());
	timer.TimeEnd("Update");
	timer.GetTime();
}

void GameState::Render()
{
	timer.TimeStart("Render");
	System::GetGraphics()->Render(_gameTimer.TotalTime(), _gameTimer.DeltaTime());
	timer.TimeEnd("Render");
}
