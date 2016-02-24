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


	_dungeon = new Dungeon(SizeOfSide, 4, 7, 0.75f, _builder);


	//_dungeon->GetPosVector();
	//_dungeon->GetUvVector();
	//_dungeon->GetIndicesVector();

	//_builder->Mesh()->CreateStaticMesh(_map, "Dungeon", _dungeon->GetPosVector(), _dungeon->GetUvVector(), _dungeon->GetIndicesVector(), _dungeon->GetSubMeshInfo());
	//_builder->Material()->BindMaterial(_map, "Shaders/GBuffer.hlsl");
	//_builder->Material()->SetEntityTexture(_map, "DiffuseMap", L"Assets/Textures/ft_stone01_c.png");
	//_builder->Material()->SetEntityTexture(_map, "NormalMap", L"Assets/Textures/ft_stone01_n.png");
	//_builder->Material()->SetMaterialProperty(_map, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	//_builder->Material()->SetMaterialProperty(_map, 0, "Metallic", 0.1f, "Shaders/GBuffer.hlsl");

	//_builder->Bounding()->CreateBBT(_map, _builder->Mesh()->GetMesh(_map));
	//_builder->Transform()->CreateTransform(_map);
	//_controller->Transform()->RotatePitch(_map, 0);


	_altar = _builder->EntityC().Create();

	_builder->Mesh()->CreateStaticMesh(_altar, "Assets/Models/cube.arf");
	_builder->Material()->BindMaterial(_altar, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetEntityTexture(_altar, "DiffuseMap", L"Assets/Textures/ft_stone01_c.png");
	_builder->Material()->SetEntityTexture(_altar, "NormalMap", L"Assets/Textures/ft_stone01_n.png");
	_builder->Material()->SetMaterialProperty(_altar, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_altar, 0, "Metallic", 0.1f, "Shaders/GBuffer.hlsl");

	_builder->Bounding()->CreateBoundingSphere(_altar, 2.0f);
	_builder->Transform()->CreateTransform(_altar);
	_controller->Transform()->SetScale(_altar, XMFLOAT3(0.5f, 0.5f, 0.5f));


	_controller->BindEventHandler(_altar, EventManager::Type::Object);
	_controller->BindEvent(_altar, EventManager::EventType::Update,
		[this]() 
	{
		if(_AI->GetLightPoolPercent() <= 0.25 && _controller->Bounding()->CheckCollision(_player->GetEntity(), _altar) != 0) // TEST
			ChangeStateTo(StateChange(new GameState()));

	});

	Entity llvl = _builder->CreateLabel(
		XMFLOAT3(0.0f, System::GetOptions()->GetScreenResolutionHeight()-50.0f, 0.0f),
		"FPS: 0",
		TextColor,
		150.0f,
		50.0f,
		"");
	_controller->BindEventHandler(llvl, EventManager::Type::Overlay);
	_controller->BindEvent(llvl, EventManager::EventType::Update,
		[llvl, this]()
	{
		_controller->Text()->ChangeText(llvl, "Light Level: " + to_string((uint)(_AI->GetLightPoolPercent()*100)));
	});


	int ax = SizeOfSide - 1, ay = SizeOfSide - 1;
	for (int i = SizeOfSide * SizeOfSide; i > 0; i--)
	{

		if (_dungeon->getTile(ax, ay) == 0)
		{
			_builder->Transform()->SetPosition(_altar, XMFLOAT3(ax - 0.5f, 0.5f, ay - 0.5f));
			_builder->Light()->BindPointLight(_altar, XMFLOAT3(ax - 0.5f, 1.5f, ay - 0.5f), 3, XMFLOAT3(1, 1, 1), 4);
			break;
		}
		ax--;
		if (!(ax % (SizeOfSide)))
		{
			ay--;
			ax = SizeOfSide - 1;
		}
	}

	std::vector<std::pair<int, int>> spot;
	for (int x = 0; x < SizeOfSide; x++)
	{
		for (int y = 0; y < SizeOfSide; y++)
		{
			if (_dungeon->getTile(x, y) == 0)
			{
				std::pair<int, int> p;
				p.first = x;
				p.second = y;
				spot.push_back(p);
			}
		}

	}
	


	std::vector<int> pre;
	for (int j = 0; j < 5; j++)
	{


		int r = rand() % spot.size();

		bool done = true;

		for (auto& pr : pre)
		{
			if (pr == r)
				done = false;
		}
		if (done)
		{

			pre.push_back(r);


			Entity wrap = _builder->EntityC().Create();
			_builder->Transform()->CreateTransform(wrap);

			Entity wep = _builder->EntityC().Create();

			_builder->Mesh()->CreateStaticMesh(wep, "Assets/Models/bth.arf");
			_controller->Mesh()->Hide(wep, 0);
			_builder->Material()->BindMaterial(wep, "Shaders/Emissive.hlsl");
			_builder->Material()->SetEntityTexture(wep, "DiffuseMap", L"Assets/Textures/default_normal.png");

			_builder->Transform()->CreateTransform(wep);

			Entity wep2 = _builder->EntityC().Create();

			_builder->Mesh()->CreateStaticMesh(wep2, "Assets/Models/bth.arf");
			_controller->Mesh()->Hide(wep2, 1);
			_builder->Material()->BindMaterial(wep2, "Shaders/Emissive.hlsl");
			_builder->Material()->SetEntityTexture(wep2, "DiffuseMap", L"Assets/Textures/default_normal.png");

			_builder->Transform()->CreateTransform(wep2);

			_builder->Transform()->BindChild(wrap, wep);
			_builder->Transform()->BindChild(wrap, wep2);

			_builder->Bounding()->CreateBoundingSphere(wrap, 0.35f);

			_builder->Transform()->SetPosition(wrap, XMFLOAT3(spot[r].first, 0.5f, spot[r].second));
			_controller->Transform()->SetScale(wep, XMFLOAT3(0.005f, 0.005f, 0.005f));
			_controller->Transform()->SetScale(wep2, XMFLOAT3(0.005f, 0.005f, 0.005f));

			_controller->BindEventHandler(wep, EventManager::Type::Object);
			_controller->BindEvent(wep, EventManager::EventType::Update,
				[wep, wep2, wrap, this]()
			{
				_controller->Transform()->RotateYaw(wep, _gameTimer.DeltaTime() * 50);
				_controller->Transform()->RotateYaw(wep2, _gameTimer.DeltaTime() * -50);
				_controller->Transform()->RotatePitch(wep2, _gameTimer.DeltaTime() * -50);
				if (_controller->Bounding()->CheckCollision(_player->GetEntity(), wrap) != 0) // TEST
				{
					int rande = (rand() % 300 + 1) / 100;
					switch (rande)
					{
					case 0:
						_player->AddWeapon(new FragBombWeapon(_builder));
						break;
					case 1:
						_player->AddWeapon(new RapidFireWeapon(_builder));
						break;
					case 2:
						_player->AddWeapon(new ShotgunWeapon(_builder));
						break;
					default:
						break;
					}


					_controller->ReleaseEntity(wep);
					_controller->ReleaseEntity(wep2);
					_controller->ReleaseEntity(wrap);
				}
			});


		}


	}
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

	_quadTree = _builder->EntityC().Create();
	const std::vector<Entity>& ents = _dungeon->GetEntites();
	_builder->Bounding()->CreateQuadTree(_quadTree, ents);

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


	//DirectX::XMVECTOR mtv;
	//const std::vector<Entity>& ents = _dungeon->GetEntites();
	//for (auto& e : ents)
	//{
	//	if (_controller->Bounding()->GetMTV(e, _player->GetEntity(), mtv))
	//	{
	//		_controller->Transform()->MoveAlongVector(_player->GetEntity(), mtv);

	//	}
	//}


	_controller->Bounding()->GetMTV(_quadTree, _player->GetEntity(),
		[this](DirectX::XMVECTOR& outMTV)
	{
		_controller->Transform()->MoveAlongVector(_player->GetEntity(), outMTV);

	});




	_ctimer.TimeEnd("Collision world");

	_ctimer.TimeStart("Player update");
	_player->Update(_gameTimer.DeltaTime());
	_ctimer.TimeEnd("Player update");

	_ctimer.TimeStart("AI");
	_AI->Update(_gameTimer.DeltaTime(), _builder->Transform()->GetPosition(_player->GetEntity()));
	_AI->CheckCollisionAgainstProjectiles(_player->GetProjectiles());
	_player->SetEnemyLightPercent(_AI->GetLightPoolPercent());
	_ctimer.TimeEnd("AI");



	_ctimer.TimeStart("Culling");
	if (System::GetInput()->IsKeyDown(VK_F))
	{
		std::vector<Entity> entites;
		_controller->Bounding()->GetEntitiesInFrustum(_controller->Camera()->GetFrustum(_player->GetEntity()), entites);
		_controller->Mesh()->SetInFrustum(entites);
	}
	_ctimer.TimeEnd("Culling");
	_ctimer.TimeEnd("Update");


	_ctimer.GetTime();

	std::string text = "Scene times\n";
	text += "\nTotal: " + to_string(_ctimer.GetAVGTPF("Update"));
	text += "\nPlayer Input: " + to_string(_ctimer.GetAVGTPF("Player input"));
	text += "\nCollision world: " + to_string(_ctimer.GetAVGTPF("Collision world"));
	text += "\nPlayer update: " + to_string(_ctimer.GetAVGTPF("Player update"));
	text += "\nAI: " + to_string(_ctimer.GetAVGTPF("AI"));
	text += "\nCulling: " + to_string(_ctimer.GetAVGTPF("Culling"));
	_controller->Text()->ChangeText(e4, text);


}

void GameState::Render()
{
	System::GetGraphics()->Render(_gameTimer.TotalTime(), _gameTimer.DeltaTime());
}
