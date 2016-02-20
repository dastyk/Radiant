#include "TestState.h"
#include "System.h"
#include "Graphics.h"
#include "Audio.h"

using namespace DirectX;
#define SizeOfSide 50



TestState::TestState() : State()
{
	
}

TestState::~TestState()
{
}


void TestState::Init()
{
	State::Init();
	_controller->SetExclusiveRenderAccess();

	//==================================
	//====	Camera and Input		====
	//================================== 
	_player = new Player(_builder); 
	_player->SetCamera();
	_builder->Light()->BindPointLight(_player->GetEntity(), XMFLOAT3(0.0f, 0.0f, 0.0f), 20.0f, XMFLOAT3(1.0f, 1.0f, 1.0f), 2.0f);

	Entity testDecal = _builder->CreateDecal(XMFLOAT3(20.0f, 10.0f, 20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f));
	Entity smallBox = _builder->CreateObject(XMVectorSet(20, 10, 20, 1), XMVectorSet(0, 0, 0,0), XMVectorSet(0.5, 0.5, 0.5, 0.0), "Assets/Models/cube.arf","Assets/Textures/ft_stone01_c.png");
	//Entity largeBox = _builder->CreateObject(XMVectorSet(20.0f, 4.0f, 20.0f, 1.0f), XMVectorSet(25.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(10.0f, 10.0f, 10.0f, 0.0f),"Assets/Models/cube.arf");

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
		[e, c, this, in]()
	{
		static bool visible = false;

		if (in->IsKeyPushed(VK_F1))
		{
			visible = (visible) ? false : true;
			_controller->ToggleVisible(e, visible);
		}
		if (visible)
			c->Text()->ChangeText(e, "FPS: " + to_string(_gameTimer.GetFps()));
	});
	_controller->ToggleVisible(e, visible);

	Entity e2 = _builder->CreateLabel(
		XMFLOAT3(0.0f, 50.0f, 0.0f),
		"MSPF: 0",
		XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f),
		150.0f,
		50.0f,
		"");
	_controller->BindEvent(e2, EventManager::EventType::Update,
		[e2, c, this, in, visible]()
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
	_controller->ToggleVisible(e2, visible);

	//==================================
	//====	Give me zee dungeon		====
	//==================================
	_map = _builder->EntityC().Create();

	
	_dungeon = new Dungeon(SizeOfSide, 4, 7, 0.75f);
	_dungeon->GetPosVector();
	_dungeon->GetUvVector();
	_dungeon->GetIndicesVector();

	_builder->Mesh()->CreateStaticMesh(_map, "Dungeon", _dungeon->GetPosVector(), _dungeon->GetUvVector(), _dungeon->GetIndicesVector(), _dungeon->GetSubMeshInfo());
	_builder->Material()->BindMaterial(_map, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetEntityTexture(_map, "DiffuseMap", L"Assets/Textures/ft_stone01_c.png");
	_builder->Material()->SetEntityTexture(_map, "NormalMap", L"Assets/Textures/ft_stone01_n.png");
	_builder->Material()->SetMaterialProperty(_map, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_map, 0, "Metallic", 0.1f, "Shaders/GBuffer.hlsl");

	_builder->Bounding()->CreateBoundingBox(_map, _builder->Mesh()->GetMesh(_map));
	_builder->Transform()->CreateTransform(_map);
	_controller->Transform()->RotatePitch(_map, 0);


	_AI = new Shodan(_builder, _dungeon, SizeOfSide);

	//Set the player to the first "empty" space we find in the map, +0.5 in x and z
	int x = 0, y = 0;
	for (int i = 0; i < SizeOfSide * SizeOfSide; i++)
	{
		
		if (_dungeon->getTile(x, y) == 0)
		{
			_player->SetPosition(XMVectorSet(x - 0.5f, 0.5f, y - 0.5f, 0.0f));
		}
		x++;
		if (!(x % (SizeOfSide)))
		{
			y++;
			x = 0;
		}
	}

	_lightLevel = 1.0f;

}

void TestState::Shutdown()
{

	delete _player;
	delete _dungeon;
	delete _AI;

}

void TestState::HandleInput()
{
	_timer.TimeStart("Input");
	_player->HandleInput(_gameTimer.DeltaTime());
	if(System::GetInput()->IsKeyPushed(VK_ESCAPE))
	{ 
		System::GetInput()->LockMouseToCenter(false);
		System::GetInput()->LockMouseToWindow(false);
		System::GetInput()->HideCursor(false);
		ChangeStateTo(StateChange(new MenuState));
	}

	//if (System::GetInput()->IsKeyPushed(VK_SPACE))
		//System::GetInstance()->ToggleFullscreen();

	_timer.TimeEnd("Input");
}

void TestState::Update()
{
	State::Update();
	_timer.TimeStart("Update");
	HandleInput();
	_player->Update(_gameTimer.DeltaTime());
	_AI->Update(_gameTimer.DeltaTime(), _builder->Transform()->GetPosition(_player->GetEntity()));
	if (_lightLevel > 0.1f)
	{
		_lightLevel -= _gameTimer.DeltaTime()*0.01;
	}
	_AI->ChangeLightLevel(max(_lightLevel, 0.1f));

	bool collideWithWorld = _builder->Bounding()->CheckCollision(_player->GetEntity(), _map);

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
		/*if (System::GetInput()->IsKeyDown(VK_SHIFT))
			_builder->GetEntityController()->Transform()->MoveUp(_player->GetEntity(), -10 * _gameTimer.DeltaTime());
		if (System::GetInput()->IsKeyDown(VK_CONTROL))
			_builder->GetEntityController()->Transform()->MoveDown(_player->GetEntity(), -10 * _gameTimer.DeltaTime());*/
	}

	_timer.TimeEnd("Update");
	_timer.GetTime();
}

void TestState::Render()
{
	System::GetGraphics()->Render( _gameTimer.TotalTime(), _gameTimer.DeltaTime() );
}
