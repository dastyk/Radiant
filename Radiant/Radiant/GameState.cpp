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

	_builder->Mesh()->CreateStaticMesh(map, "Dungeon", dun.GetPosVector(), dun.GetUvVector(), dun.GetIndicesVector(), dun.GetSubMeshInfo());
	_builder->Material()->BindMaterial(map, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetEntityTexture(map, "DiffuseMap", L"Assets/Textures/ft_stone01_c.png");
	_builder->Material()->SetEntityTexture(map, "NormalMap", L"Assets/Textures/ft_stone01_n.png");
	
	_builder->Material()->SetMaterialProperty(map, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(map, "Metallic", 0.1f, "Shaders/GBuffer.hlsl");
	_builder->Bounding()->CreateBoundingBox(map, _builder->Mesh()->GetMesh(map));
	_builder->Transform()->CreateTransform(map);
	_controller->Transform()->RotatePitch(map, 0);

	uint i = 15;
	while (i > 0)
	{
		int x = (rand() % (2400-100) + 100)/100;
		int y = (rand() % (2400 - 100) + 100) / 100;
		if (dun.getTile(x, y) == 0)
		{
			Entity e = _builder->EntityC().Create();
			_builder->Transform()->CreateTransform(e);
			float r = (rand() % 255) / 255.0f;
			float g = (rand() % 255) / 255.0f;
			float b = (rand() % 255) / 255.0f;
			_builder->Light()->BindPointLight(e, XMFLOAT3((float)x, 2.0f, (float)y), 2.0f, XMFLOAT3(r, g, b), 5.0f);
			_builder->Light()->SetAsVolumetric(e, true);
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


	Entity e = _builder->CreateLabel(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		"FPS: 0",
		XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f),
		150.0f,
		50.0f,
		"");
	auto c = _controller;
	auto in = System::GetInput();
	bool visible = false;

	_controller->BindEvent(e, EventManager::EventType::Update,
		[e, c,this,in]()
	{
		static bool visible = false;

		if (in->IsKeyPushed(VK_F1))
		{
			visible = (visible) ? false : true;
			_controller->ToggleVisible(e, visible);
		}
		if(visible)
			c->Text()->ChangeText(e, "FPS: " + to_string(_gameTimer.GetFps()));
	});

	Entity e2 = _builder->CreateLabel(
		XMFLOAT3(0.0f, 50.0f, 0.0f),
		"MSPF: 0",
		XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f),
		150.0f,
		50.0f,
		"");
	_controller->BindEvent(e2, EventManager::EventType::Update,
		[e2, c, this,in, visible]()
	{
		static bool visible = false;

		if (in->IsKeyPushed(VK_F2))
		{
			visible = (visible) ? false : true;
			_controller->ToggleVisible(e2, visible);
		}
		if (visible)
			c->Text()->ChangeText(e2, "MSPF: " + to_string(_gameTimer.GetMspf()));
	});

	_controller->ToggleVisible(e, visible);
	_controller->ToggleVisible(e2, visible);


}

void GameState::Shutdown()
{
	State::Shutdown();
	if (!_passed)
	{
		delete _enemies;
		delete _player;
		
	}	
}

void GameState::HandleInput()
{
	timer.TimeStart("Input");
	if (System::GetInput()->IsKeyPushed(VK_ESCAPE))
	{

		System::GetInput()->LockMouseToCenter(false);
		System::GetInput()->LockMouseToWindow(false);
		System::GetInput()->HideCursor(false);
		ChangeStateTo(StateChange(new MenuState));
	}
	_player->HandleInput(_gameTimer.DeltaTime());
	timer.TimeEnd("Input");
}

void GameState::Update()
{
	
	HandleInput();
	timer.TimeStart("Update");
	State::Update();
	bool collideWithWorld = _builder->Bounding()->CheckCollision(_player->GetEntity(), map);

	if (collideWithWorld) // Naive and simple way, but works for now
	{
		if (System::GetInput()->IsKeyDown(VK_W))
			_builder->GetEntityController()->Transform()->MoveForward(_player->GetEntity(), -10 * _gameTimer.DeltaTime());
		if (System::GetInput()->IsKeyDown(VK_S))
			_builder->GetEntityController()->Transform()->MoveBackward(_player->GetEntity(), -10 * _gameTimer.DeltaTime());
		if (System::GetInput()->IsKeyDown(VK_A))
			_builder->GetEntityController()->Transform()->MoveLeft(_player->GetEntity(), -10 * _gameTimer.DeltaTime());
		if (System::GetInput()->IsKeyDown(VK_D))
			_builder->GetEntityController()->Transform()->MoveRight(_player->GetEntity(), -10 * _gameTimer.DeltaTime());
		if (System::GetInput()->IsKeyDown(VK_SHIFT))
			_builder->GetEntityController()->Transform()->MoveUp(_player->GetEntity(), -10 * _gameTimer.DeltaTime());
		if (System::GetInput()->IsKeyDown(VK_CONTROL))
			_builder->GetEntityController()->Transform()->MoveDown(_player->GetEntity(), -10 * _gameTimer.DeltaTime());
	}


 	for (int i = 0; i < _enemies->Size(); i++)
	{
		_enemies->GetCurrentElement()->Update(_gameTimer.DeltaTime());
		_enemies->MoveCurrent();
	}

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
