#include "TestState.h"
#include "System.h"
#include "Graphics.h"
#include "Audio.h"

using namespace DirectX;



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


	_dungeon = new Dungeon(50, 4, 7, 0.75f);
	_dungeon->GetPosVector();
	_dungeon->GetUvVector();
	_dungeon->GetIndicesVector();

	_builder->Mesh()->CreateStaticMesh(_map, "Dungeon", _dungeon->GetPosVector(), _dungeon->GetUvVector(), _dungeon->GetIndicesVector());
	_builder->Material()->BindMaterial(_map, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetEntityTexture(_map, "DiffuseMap", L"Assets/Textures/ft_stone01_c.png");
	_builder->Material()->SetEntityTexture(_map, "NormalMap", L"Assets/Textures/ft_stone01_n.png");
	_builder->Material()->SetMaterialProperty(_map, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_map, 0, "Metalic", 0.1f, "Shaders/GBuffer.hlsl");


	_AI = new Shodan(_builder, _dungeon, 50);

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

	if (System::GetInput()->IsKeyPushed(VK_SPACE))
		System::GetInstance()->ToggleFullscreen();
	
	_timer.TimeEnd("Input");
}

void TestState::Update()
{
	State::Update();
	_timer.TimeStart("Update");
	HandleInput();
	_player->Update(_gameTimer.DeltaTime());
	_AI->Update(_gameTimer.DeltaTime());

	_timer.TimeEnd("Update");
	_timer.GetTime();
}

void TestState::Render()
{
	System::GetGraphics()->Render( _gameTimer.TotalTime(), _gameTimer.DeltaTime() );
}
