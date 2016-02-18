#include "GameState.h"
#include "System.h"
#include "Graphics.h"

using namespace DirectX;
#define SizeOfSide 50


GameState::GameState() : State()
{



	
}

GameState::~GameState()
{

}

void GameState::Init()
{
	XMFLOAT4 TextColor = XMFLOAT4(0.56f, 0.26f, 0.15f, 1.0f);


	//==================================
	//====	Create All Things		====
	//==================================
	try { _player = new Player(_builder); }
	catch (std::exception& e) { e; throw ErrorMsg(3000005, L"Failed to create a player in the GameState."); }

	//==================================
	//====		Set Camera			====
	//==================================
	_player->SetCamera();

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

	_builder->Bounding()->CreateBBT(_map, _builder->Mesh()->GetMesh(_map));
	_builder->Transform()->CreateTransform(_map);
	_controller->Transform()->RotatePitch(_map, 0);


	//==================================
	//====	Give me zee AI			====
	//==================================

	_AI = new Shodan(_builder, _dungeon, SizeOfSide);

	//Set the player to the first "empty" space we find in the map, +0.5 in x and z
	int x = 0, y = 0;
	for (int i = 0; i < SizeOfSide * SizeOfSide; i++)
	{

		if (_dungeon->getTile(x, y) == 0)
		{
			_player->SetPosition(XMVectorSet(x + 0.5f, 0.5f, y + 0.5f, 0.0f));
			break;
		}
		x++;
		if (!(x % (SizeOfSide)))
		{
			y++;
			x = 0;
		}
	}

	//==================================
	//====		Set Input data		====
	//==================================


	Entity e = _builder->CreateLabel(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		"FPS: 0",
		TextColor,
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
	_controller->BindEventHandler(e, EventManager::Type::Overlay);
	Entity e2 = _builder->CreateLabel(
		XMFLOAT3(0.0f, 50.0f, 0.0f),
		"MSPF: 0",
		TextColor,
		150.0f,
		50.0f,
		"");
	_controller->BindEventHandler(e2, EventManager::Type::Overlay);
	_controller->BindEvent(e2, EventManager::EventType::Update,
		[e2, c, this, in]()
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

	auto g = System::GetGraphics();

	Entity e3 = _builder->CreateLabel(
		XMFLOAT3(0.0f, 100.0f, 0.0f),
		"Average time per frame\n",
		TextColor,
		150.0f,
		50.0f,
		"");
	_controller->BindEventHandler(e3, EventManager::Type::Overlay);
	_controller->BindEvent(e3, EventManager::EventType::Update,
		[e3, c, this, in,g]()
	{
		static bool visible = false;

		if (in->IsKeyPushed(VK_F3))
		{
			visible = (visible) ? false : true;
			_controller->ToggleVisible(e3, visible);
		}
		if (visible)
			c->Text()->ChangeText(e3, "Average time per frame\n" + g->GetAVGTPFTimes() );
	});

	e4 = _builder->CreateLabel(
		XMFLOAT3(550.0f, 0.0f, 0.0f),
		"Average time per frame\n",
		TextColor,
		150.0f,
		50.0f,
		"");
	_controller->BindEventHandler(e4, EventManager::Type::Overlay);
	_controller->BindEvent(e4, EventManager::EventType::Update,
		[c, this, in]()
	{
		static bool visible = false;

		if (in->IsKeyPushed(VK_F4))
		{
			visible = (visible) ? false : true;
			_controller->ToggleVisible(e4, visible);
		}
	});

	_controller->ToggleVisible(e, visible);
	_controller->ToggleVisible(e2, visible);
	_controller->ToggleVisible(e3, visible);
	_controller->ToggleVisible(e4, visible);
}

void GameState::Shutdown()
{
	State::Shutdown();

	SAFE_DELETE(_player);
	SAFE_DELETE(_dungeon);
	SAFE_DELETE(_AI);
}

void GameState::Update()
{
	_ctimer.TimeStart("Update");
	State::Update();

	if (System::GetInput()->IsKeyPushed(VK_ESCAPE))
	{

		System::GetInput()->LockMouseToCenter(false);
		System::GetInput()->LockMouseToWindow(false);
		System::GetInput()->HideCursor(false);
		ChangeStateTo(StateChange(new MenuState));
	}
	_ctimer.TimeStart("Player input");
	_player->HandleInput(_gameTimer.DeltaTime());
	_ctimer.TimeEnd("Player input");


	_ctimer.TimeStart("Collision world");
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
		if (System::GetInput()->IsKeyDown(VK_SHIFT))
			_builder->GetEntityController()->Transform()->MoveUp(_player->GetEntity(), -10 * _gameTimer.DeltaTime());
		if (System::GetInput()->IsKeyDown(VK_CONTROL))
			_builder->GetEntityController()->Transform()->MoveDown(_player->GetEntity(), -10 * _gameTimer.DeltaTime());
	}
	_ctimer.TimeEnd("Collision world");

	_ctimer.TimeStart("Player update");					
	_player->Update(_gameTimer.DeltaTime());
	_ctimer.TimeEnd("Player update");

	_ctimer.TimeStart("AI");
	_AI->Update(_gameTimer.DeltaTime(), _builder->Transform()->GetPosition(_player->GetEntity()));
	_ctimer.TimeEnd("AI");

	_ctimer.TimeEnd("Update");

	_ctimer.GetTime();

	std::string text = "Scene times\n";
	text += "\nTotal: " + to_string(_ctimer.GetAVGTPF("Update"));
	text += "\nPlayer Input: " + to_string(_ctimer.GetAVGTPF("Player input"));
	text += "\nCollision world: " + to_string(_ctimer.GetAVGTPF("Collision world"));
	text += "\nPlayer update: " + to_string(_ctimer.GetAVGTPF("Player update"));
	text += "\nAI: " + to_string(_ctimer.GetAVGTPF("AI"));
	_controller->Text()->ChangeText(e4, text);
}

void GameState::Render()
{
	System::GetGraphics()->Render(_gameTimer.TotalTime(), _gameTimer.DeltaTime());
}
