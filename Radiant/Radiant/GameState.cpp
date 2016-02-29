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

	auto o = System::GetOptions();
	float width = (float)o->GetScreenResolutionWidth();
	float height = (float)o->GetScreenResolutionHeight();
	auto i = System::GetInput();
	auto a = System::GetInstance()->GetAudio();

	//==================================
	//====	Create All Things		====
	//==================================
	try { _player = new Player(_builder); }
	catch (std::exception& e) { e; throw ErrorMsg(3000005, L"Failed to create a player in the GameState."); }

	//==================================
	//====	Give me zee dungeon		====
	//==================================
	_map = _builder->EntityC().Create();


	_dungeon = new Dungeon(SizeOfSide, 4, 7, 0.75f, _builder);

	//==================================
	//====		Set Camera			====
	//==================================
	_player->SetCamera();



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

	Entity ndl = _builder->CreateLabel(
		XMFLOAT3(width/2.0f - 300.0f, height /2.0f - 50.0f, 0.0f),
		"You must collect more light!",
		TextColor,
		300.0f,
		50.0f,
		"");
	_controller->ToggleVisible(ndl, false);
	Entity bdone = _builder->CreateButton(
		XMFLOAT3(System::GetOptions()->GetScreenResolutionWidth() / 2.0f - 75.0f, System::GetOptions()->GetScreenResolutionHeight() / 2.0f - 50.0f, 0.0f),
		"Proceed",
		TextColor,
		250.0f,
		50.0f,
		"",
		[i, a]()
	{
		a->PlaySoundEffect(L"menuclick.wav", 1);
		i->LockMouseToCenter(true);
		i->LockMouseToWindow(true);
		i->HideCursor(true);
		ChangeStateTo(StateChange(new GameState()));
	});
	_controller->ToggleVisible(bdone, false);
	_controller->ToggleEventChecking(bdone, false);

	_controller->BindEventHandler(_altar, EventManager::Type::Object);
	_controller->BindEvent(_altar, EventManager::EventType::Update,
		[this, ndl, bdone,i]()
	{
		static bool in = false;
		if (_controller->Bounding()->CheckCollision(_player->GetEntity(), _altar) != 0) // TEST
		{
			if (!in)
			{
				if (_AI->GetLightPoolPercent() <= 0.25)
				{
					_controller->ToggleVisible(ndl, false);
					_controller->ToggleVisible(bdone, true);
					_controller->ToggleEventChecking(bdone, true);
					i->LockMouseToCenter(false);
					i->LockMouseToWindow(true);
					i->HideCursor(false);
		
					in = true;
				}
				else
	{
					_controller->ToggleVisible(ndl, true);
					_controller->ToggleVisible(bdone, false);
					_controller->ToggleEventChecking(bdone, false);
					i->LockMouseToCenter(true);
					i->LockMouseToWindow(true);
					i->HideCursor(true);

					in = true;
				}
			}
		}
		else
		{
			if (in)
			{
				_controller->ToggleVisible(ndl, false);
				_controller->ToggleVisible(bdone, false);
				_controller->ToggleEventChecking(bdone, false);
				i->LockMouseToCenter(true);
				i->LockMouseToWindow(true);
				i->HideCursor(true);
				in = false;
			}
		}
	});


	Entity llvl = _builder->CreateLabel(
		XMFLOAT3(0.0f, System::GetOptions()->GetScreenResolutionHeight() - 50.0f, 0.0f),
		"FPS: 0",
		TextColor,
		150.0f,
		50.0f,
		"");

	_controller->BindEventHandler(llvl, EventManager::Type::Overlay);
	_controller->BindEvent(llvl, EventManager::EventType::Update,
		[llvl, this]()
	{
		static float prev = _AI->GetLightPoolPercent();
		static float curr = prev;

		curr = _AI->GetLightPoolPercent();
		if (curr < prev)
		{
			prev -= _gameTimer.DeltaTime()*0.1;
			_controller->Text()->ChangeText(llvl, "Light Level: " + to_string((uint)(prev * 100)));
			//_controller->Camera()->SetDrawDistance(_player->GetEntity(), (1.0f - prev + 0.25) * 25);
			_controller->Camera()->SetViewDistance(_player->GetEntity(), (1.0f - prev)*15.0 + 6.0f);
			_controller->Light()->ChangeLightRange(_player->GetEntity(), (1.0f - prev)*15.0 + 1.0f);
		}
		else
		{
			prev = curr;
		}
	});


	FreePositions p = _dungeon->GetunoccupiedSpace();
	_builder->Transform()->SetPosition(_altar, XMFLOAT3(p.x, 0.25f, p.y));
	_builder->Light()->BindPointLight(_altar, XMFLOAT3(p.x, 1.5f, p.y), 1, XMFLOAT3(1, 1, 1), 4);


	for (int j = 0; j < 5; j++)
	{
		p = _dungeon->GetunoccupiedSpace();

			Entity wrap = _builder->EntityC().Create();
			_builder->Transform()->CreateTransform(wrap);

			Entity wep = _builder->EntityC().Create();

			_builder->Mesh()->CreateStaticMesh(wep, "Assets/Models/bth.arf");
			_controller->Mesh()->Hide(wep, 0);
			_builder->Material()->BindMaterial(wep, "Shaders/Emissive.hlsl");


			_builder->Transform()->CreateTransform(wep);


			Entity wep2 = _builder->EntityC().Create();

			_builder->Mesh()->CreateStaticMesh(wep2, "Assets/Models/bth.arf");
			_controller->Mesh()->Hide(wep2, 1);
			_builder->Material()->BindMaterial(wep2, "Shaders/Emissive.hlsl");


			_builder->Transform()->CreateTransform(wep2);

			_builder->Transform()->BindChild(wrap, wep);
			_builder->Transform()->BindChild(wrap, wep2);

		_builder->Bounding()->CreateBoundingSphere(wrap, 0.20f);
		_builder->Bounding()->CreateBoundingSphere(wep, 0.20f);
		_builder->Bounding()->CreateBoundingSphere(wep2, 0.20f);

		_builder->Transform()->SetPosition(wrap, XMFLOAT3(p.x, 0.5f, p.y));
		_controller->Transform()->SetScale(wep, XMFLOAT3(0.0025f, 0.0025f, 0.0025f));
		_controller->Transform()->SetScale(wep2, XMFLOAT3(0.0025f, 0.0025f, 0.0025f));

		_controller->BindEventHandler(wep, EventManager::Type::Object);

		int rande = (rand() % 300) / 100;
		switch (rande)
		{
		case 0:
		{
			_builder->Material()->SetEntityTexture(wep, "DiffuseMap", L"Assets/Textures/fragguntex.dds");
			_builder->Material()->SetEntityTexture(wep2, "DiffuseMap", L"Assets/Textures/fragguntex.dds");
			break;
		}
		case 1:
		{

			_builder->Material()->SetEntityTexture(wep, "DiffuseMap", L"Assets/Textures/rapidguntex.dds");
			_builder->Material()->SetEntityTexture(wep2, "DiffuseMap", L"Assets/Textures/rapidguntex.dds");
			break;
		}
		case 2:
		{
			_builder->Material()->SetEntityTexture(wep, "DiffuseMap", L"Assets/Textures/shotguntex.dds");
			_builder->Material()->SetEntityTexture(wep2, "DiffuseMap", L"Assets/Textures/shotguntex.dds");
		}
			break;
		default:
			break;
		}


		_controller->BindEvent(wep, EventManager::EventType::Update,
			[wep, wep2, wrap, this,rande]()
		{
			_controller->Transform()->RotateYaw(wep, _gameTimer.DeltaTime() * 50);
			_controller->Transform()->RotateYaw(wep2, _gameTimer.DeltaTime() * -50);
			_controller->Transform()->RotatePitch(wep2, _gameTimer.DeltaTime() * -50);
			if (_controller->Bounding()->CheckCollision(_player->GetEntity(), wrap) != 0) // TEST
			{

				_player->AddWeapon(rande + 1);

				_controller->ReleaseEntity(wep);
				_controller->ReleaseEntity(wep2);
				_controller->ReleaseEntity(wrap);
			}
		});
		





	}
	//==================================
	//====	Give me zee AI			====
	//==================================

	_AI = new Shodan(_builder, _dungeon, SizeOfSide, _player);
	_controller->Text()->ChangeText(llvl, "Light Level: " + to_string((uint)(_AI->GetLightPoolPercent() * 100)));
	_controller->Camera()->SetDrawDistance(_player->GetEntity(), 25.0f);
	_controller->Camera()->SetViewDistance(_player->GetEntity(), (1.0f - _AI->GetLightPoolPercent())*15.0 + 6.0f);
	_controller->Light()->ChangeLightRange(_player->GetEntity(), (1.0f - _AI->GetLightPoolPercent())*15.0 + 1.0f);
	//_controller->Light()->ChangeLightRange(_player->GetEntity(), (1.2f - _AI->GetLightPoolPercent())*10.0);
	//_controller->Camera()->SetDrawDistance(_player->GetEntity(), 35);
	p = _dungeon->GetunoccupiedSpace();


	//Set the player to the first "empty" space we find in the map, +0.5 in x and z

	_player->SetPosition(XMVectorSet(p.x, 0.5f, p.y, 1.0f));


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
		[e3, c, this, in, g]()
	{
		static bool visible = false;

		if (in->IsKeyPushed(VK_F3))
		{
			visible = (visible) ? false : true;
			_controller->ToggleVisible(e3, visible);
		}
		if (visible)
			c->Text()->ChangeText(e3, "Average time per frame\n" + g->GetAVGTPFTimes());
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



	//Power* testPower = new RandomBlink(_builder, _player->GetEntity(), _dungeon->GetFreePositions());
	Power* testPower = new LockOnStrike(_builder, _player->GetEntity(), _AI->GetEnemyList());
	_player->SetPower(testPower);
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


	_controller->Bounding()->GetMTV(_quadTree, _player->GetEntity(),
		[this](DirectX::XMVECTOR& outMTV, const Entity& entity)
	{
		_controller->Transform()->MoveAlongVector(_player->GetEntity(), outMTV);

	});




	_ctimer.TimeEnd("Collision world");

	_ctimer.TimeStart("Player update");
	_player->Update(_gameTimer.DeltaTime());


	const std::vector<Entity>& ents = _dungeon->GetEntites();
	static float prev2 = _player->GetHealth();
	static float curr2 = prev2;

	curr2 = _player->GetHealth();
	if (curr2 < prev2)
	{
		float delta = prev2 - curr2;
		prev2 -= _gameTimer.DeltaTime()*delta*5;
		for (auto& e : ents)
		{

			_controller->Material()->SetMaterialProperty(e, "EmissiveIntensity", prev2 / 100.0f, "Shaders/GBufferEmissive.hlsl");

		}
	}
	else
	{
		prev2 = curr2;
	}


	if (curr2 <= 0.0f)
	{
		System::GetInput()->LockMouseToCenter(false);
		System::GetInput()->LockMouseToWindow(false);
		System::GetInput()->HideCursor(false);
		ChangeStateTo(StateChange(new MenuState));
	}



	_ctimer.TimeEnd("Player update");

	_ctimer.TimeStart("AI");
	_AI->Update(_gameTimer.DeltaTime(), _builder->Transform()->GetPosition(_player->GetEntity()));
	std::vector<Projectile*>& ps = _player->GetProjectiles();
	for (auto& p : ps)
	{
		_controller->Bounding()->GetMTV(_quadTree, p->GetEntity(),
			[this,p](DirectX::XMVECTOR& outMTV, const Entity& entity)
		{
			//_controller->Transform()->MoveAlongVector(p->GetEntity(), outMTV);
			XMVECTOR pos = _builder->Transform()->GetPosition(p->GetEntity()) + outMTV*1.1f;
			XMVECTOR rot = _builder->Transform()->GetRotation(p->GetEntity());
			XMFLOAT3 fpos;
			XMStoreFloat3(&fpos, pos);
			XMFLOAT3 frot;
			XMStoreFloat3(&frot, rot);
			XMFLOAT3 dir;
			XMStoreFloat3(&dir, XMVector3Normalize( -outMTV));
			_builder->CreateDecal(fpos, frot, XMFLOAT3(0.2f, 0.2f, 1.0f),
				"Assets/Textures/Damage_Dif.png", "Assets/Textures/Damage_NM.png");
			//Entity e = _builder->EntityC().Create();
			//_builder->Light()->BindAreaRectLight(e, fpos, dir, 5.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), 0.5f, 0.5f, XMFLOAT3(1.0f, 0.0f, 0.0f), 5.0f);

			//_builder->Decal()->BindDecal(p->GetEntity());								
			//_builder->Decal()->SetColorTexture(p->GetEntity(), L"Assets/Textures/per.png");
			p->CollideWithEntity(outMTV, entity);
			//_controller->ReleaseEntity(entity);  //hehe minecraft
			
			/*XMVECTOR dir = _controller->Transform()->GetRotation(p->GetEntity());

			XMVECTOR newDir = XMVector3Dot(dir, XMVector3Normalize(outMTV));

			_controller->Transform()->SetRotation(p->GetEntity(), newDir);*/
		});
	}
	_AI->CheckCollisionAgainstProjectiles(_player->GetProjectiles());
	_player->SetEnemyLightPercent(_AI->GetLightPoolPercent());
	_ctimer.TimeEnd("AI");



	_ctimer.TimeStart("Culling");
	if (!System::GetInput()->IsKeyDown(VK_F))
	{
		static uint framecount = 10;
		framecount++;
		if (framecount > 2)
		{
			std::vector<Entity> entites;
			_controller->Bounding()->GetEntitiesInFrustumNoQuadTree(_controller->Camera()->GetFrustum(_player->GetEntity()), entites);
		//	_controller->Light()->SetInFrustum(entites);
			_controller->Bounding()->GetEntitiesInFrustum(_controller->Camera()->GetFrustum(_player->GetEntity()), entites);
			_controller->Mesh()->SetInFrustum(entites);
			framecount = 0;
		}
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
