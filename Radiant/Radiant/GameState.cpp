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

	_controller->ToggleVisible(e, visible);
	_controller->ToggleVisible(e2, visible);


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
	State::Update();

	if (System::GetInput()->IsKeyPushed(VK_ESCAPE))
	{

		System::GetInput()->LockMouseToCenter(false);
		System::GetInput()->LockMouseToWindow(false);
		System::GetInput()->HideCursor(false);
		ChangeStateTo(StateChange(new MenuState));
	}
	_player->HandleInput(_gameTimer.DeltaTime());

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
	_player->Update(_gameTimer.DeltaTime());


	_AI->Update(_gameTimer.DeltaTime(), _builder->Transform()->GetPosition(_player->GetEntity()));
}

void GameState::Render()
{
	System::GetGraphics()->Render(_gameTimer.TotalTime(), _gameTimer.DeltaTime());
}
