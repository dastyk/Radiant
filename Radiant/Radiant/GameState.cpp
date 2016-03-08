#include "GameState.h"
#include "System.h"
#include "Graphics.h"
#include "PowersHeader.h"

using namespace DirectX;
#define SizeOfSide 50


GameState::GameState() : State(),_lightRemaning(0.0f), _lightTreshold(0.0f), _timeSinceLastSound(0.0f), _currentPreQuoteSound(0), _currentAfterQuoteSound(0), _map(Entity()), e4(Entity()), _altar(Entity()), _quadTree(Entity())
{



	
}

GameState::~GameState()
{

}

void GameState::Init()
{
	XMFLOAT4 TextColor = XMFLOAT4(0.56f, 0.26f, 0.15f, 1.0f);
	_timeSinceLastSound = 100;
	_currentPreQuoteSound = 0;
	_currentAfterQuoteSound = 0;

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


	FreePositions p = _dungeon->GetunoccupiedSpace();
	_altar = _builder->CreateObject(
		XMVectorSet((float)p.x, 0.25f, (float)p.y,1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f,0.0f),
		XMVectorSet(0.5f, 0.5f, 0.5f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/Floor_Dif.png",
		"Assets/Textures/Floor_NM.png",
		"Assets/Textures/Floor_Disp.png",
		"Assets/Textures/Floor_Roughness.png");
	_builder->Material()->SetMaterialProperty(_altar, "ParallaxBias", -0.05f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_altar, "ParallaxScaling", 0.12f, "Shaders/GBuffer.hlsl");


	_builder->Bounding()->CreateBoundingSphere(_altar, 2.0f);



	Entity ali = _builder->EntityC().Create();
	_builder->Light()->BindPointLight(ali, XMFLOAT3(0.0f,1.5f,0.0f), 3.0f, XMFLOAT3(1.0f, 1.0f, 1.0f), 10.0f);
	_builder->Light()->ChangeLightBlobRange(ali, 2.0f);
	_builder->Transform()->CreateTransform(ali);
	_builder->Transform()->SetPosition(ali, XMFLOAT3(0.0f, 2.5f, 0.0f));
	_builder->Transform()->BindChild(_altar, ali);

	/*Entity ali = _builder->CreateHealingLight(XMFLOAT3(0.0f,5.0f,0.0f),XMFLOAT3(90.0f,0.0f,0.0f),XMFLOAT3(1.0f,1.0f,1.0f),2.0f, XMConvertToRadians(50.0f), XMConvertToRadians(30.0f), 6.0f);
	
	_builder->Transform()->BindChild(_altar, ali);
*/
	_builder->Transform()->SetPosition(_altar, XMFLOAT3((float)p.x, 0.25f, (float)p.y));

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
		[this, ndl, bdone, i]()
	{
		static bool in = false;
		if (_controller->Bounding()->CheckCollision(_player->GetEntity(), _altar) != 0) // TEST
		{
			if (!in)
			{
				if (_AI->GetLightPoolPercent() <= 0.25)
				{
					// HERE HERE HERE
					if (_timeSinceLastSound >= 10.0f)
					{
						if (_currentAfterQuoteSound == 0)
						{
							System::GetAudio()->PlaySoundEffect(L"AltarAfter1.wav", 1.0f);
						}
						else if (_currentAfterQuoteSound == 1)
						{
							System::GetAudio()->PlaySoundEffect(L"AltarAfter2.wav", 1.0f);
						}
						else if (_currentAfterQuoteSound == 2)
						{
							System::GetAudio()->PlaySoundEffect(L"AltarAfter3.wav", 1.0f);
						}


						_currentAfterQuoteSound = (_currentAfterQuoteSound + 1) % 3;
						_timeSinceLastSound = 0.0f;
					}

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
					// change things sounds
					if (_timeSinceLastSound >= 10.0f)
					{
						if (_currentPreQuoteSound == 0)
						{
							System::GetAudio()->PlaySoundEffect(L"AltarPre1.wav", 1.0f);
						}
						else if (_currentPreQuoteSound == 1)
						{
							System::GetAudio()->PlaySoundEffect(L"AltarPre2.wav", 1.0f);
						}
						else if (_currentPreQuoteSound == 2)
						{
							System::GetAudio()->PlaySoundEffect(L"AltarPre3.wav", 1.0f);
						}
						else if (_currentPreQuoteSound == 3)
						{
							System::GetAudio()->PlaySoundEffect(L"AltarPre4.wav", 1.0f);
						}
						else
						{
							System::GetAudio()->PlaySoundEffect(L"AltarPre5.wav", 1.0f);
						}

						_currentPreQuoteSound++;
						_timeSinceLastSound = 0.0f;
					}

					_controller->ToggleVisible(ndl, true);
					_controller->ToggleVisible(bdone, false);
					_controller->ToggleEventChecking(bdone, false);
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
		static float prev = _AI->GetLightPoolPercent() * 1000;
		static float curr = prev;

		curr = _AI->GetLightPoolPercent() * 1000;
		if (curr < prev)
		{
			float diff = prev - curr;
			prev -= _gameTimer.DeltaTime()*2*diff+1;
			_controller->Text()->ChangeText(llvl, "Light Collected: " + to_string((uint)(1000 - prev)));
			//_controller->Camera()->SetDrawDistance(_player->GetEntity(), (1.0f - prev + 0.25) * 25);
			_controller->Camera()->SetViewDistance(_player->GetEntity(), (1.0f - prev / 1000.0f)*15.0f + 6.0f);
			_controller->Light()->ChangeLightRange(_player->GetEntity(), (1.0f - prev / 1000.0f)*15.0f + 1.0f);
		}
		else
		{
			prev = curr;
		}
	});




	for (int j = 0; j < 10; j++)
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

		_builder->Transform()->SetPosition(wrap, XMFLOAT3((float)p.x, 0.5f, (float)p.y));
		_controller->Transform()->SetScale(wep, XMFLOAT3(0.0025f, 0.0025f, 0.0025f));
		_controller->Transform()->SetScale(wep2, XMFLOAT3(0.0025f, 0.0025f, 0.0025f));

		_controller->BindEventHandler(wep, EventManager::Type::Object);

		int rande = (rand() % 400) / 100;
		switch (rande)
		{
		case 0:
		{
			_builder->Material()->SetEntityTexture(wep, "DiffuseMap", L"Assets/Textures/bouncetex.dds");
			_builder->Material()->SetEntityTexture(wep2, "DiffuseMap", L"Assets/Textures/bouncetex.dds");
			break;
		}
		case 1:
		{

			_builder->Material()->SetEntityTexture(wep, "DiffuseMap", L"Assets/Textures/fragguntex.dds");
			_builder->Material()->SetEntityTexture(wep2, "DiffuseMap", L"Assets/Textures/fragguntex.dds");
			break;
		}
		case 2:
		{


			_builder->Material()->SetEntityTexture(wep, "DiffuseMap", L"Assets/Textures/rapidguntex.dds");
			_builder->Material()->SetEntityTexture(wep2, "DiffuseMap", L"Assets/Textures/rapidguntex.dds");
		}
		break;
		case 3:
		{

			_builder->Material()->SetEntityTexture(wep, "DiffuseMap", L"Assets/Textures/shotguntex.dds");
			_builder->Material()->SetEntityTexture(wep2, "DiffuseMap", L"Assets/Textures/shotguntex.dds");
		}
		break;
		default:
			break;
		}


		_controller->BindEvent(wep, EventManager::EventType::Update,
			[wep, wep2, wrap, this,rande,a]()
		{

			_controller->Transform()->RotateYaw(wep, _gameTimer.DeltaTime() * 50);
			_controller->Transform()->RotateYaw(wep2, _gameTimer.DeltaTime() * -50);
			_controller->Transform()->RotatePitch(wep2, _gameTimer.DeltaTime() * -50);
			if (_controller->Bounding()->CheckCollision(_player->GetEntity(), wrap) != 0) // TEST
			{
				a->PlaySoundEffect(L"weppickup.wav", 1.0f);
				_player->AddWeapon(rande + 1);

				_controller->ReleaseEntity(wep);
				_controller->ReleaseEntity(wep2);
				_controller->ReleaseEntity(wrap);
			}
		});
		





	}

	//_controller->Light()->ChangeLightRange(_player->GetEntity(), (1.2f - _AI->GetLightPoolPercent())*10.0);
	//_controller->Camera()->SetDrawDistance(_player->GetEntity(), 35);
	p = _dungeon->GetunoccupiedSpace();


	//Set the player to the first "empty" space we find in the map, +0.5 in x and z

	_player->SetPosition(XMVectorSet((float)p.x, 0.5f, (float)p.y, 1.0f));

	//==================================
	//====	Give me zee AI			====
	//==================================
	_AI = new Shodan(_builder, _dungeon, SizeOfSide, _player);
	_controller->Text()->ChangeText(llvl, "Light Collected: 0");
	//_controller->Camera()->SetDrawDistance(_player->GetEntity(), 25.0f);
	_controller->Camera()->SetViewDistance(_player->GetEntity(), (1.0f - _AI->GetLightPoolPercent())*15.0f + 6.0f);
	_controller->Light()->ChangeLightRange(_player->GetEntity(), (1.0f - _AI->GetLightPoolPercent())*15.0f + 1.0f);

	_quadTree = _builder->EntityC().Create();
	const std::vector<Entity>& walls = _dungeon->GetWalls();
	const std::vector<Entity>& fr = _dungeon->GetFloorRoof();

	std::vector<Entity> vect;
	vect.insert(vect.begin(), walls.begin(), walls.end());
	vect.insert(vect.begin(), fr.begin(), fr.end());

	_builder->Bounding()->CreateQuadTree(_quadTree, vect);

	//for (uint i = 0; i < 10; i++)
	//{
	//	p = _dungeon->GetunoccupiedSpace();

	//	_builder->CreateHealingLight(XMFLOAT3(p.x, 3.0f, p.y), XMFLOAT3(90.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 5.0f, XMConvertToRadians(30.0f), XMConvertToRadians(20.0f), 4.0f);
	//}
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

	bool visible = false;

	_controller->BindEvent(e, EventManager::EventType::Update,
		[e, this, i]()
	{
		static bool visible = false;

		if (i->IsKeyPushed(VK_F1))
		{
			visible = (visible) ? false : true;
			_controller->ToggleVisible(e, visible);
		}
		if (visible)
			_controller->Text()->ChangeText(e, "FPS: " + to_string(_gameTimer.GetFps()));
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
		[e2, this, i]()
	{
		static bool visible = false;

		if (i->IsKeyPushed(VK_F2))
		{
			visible = (visible) ? false : true;
			_controller->ToggleVisible(e2, visible);
		}
		if (visible)
			_controller->Text()->ChangeText(e2, "MSPF: " + to_string(_gameTimer.GetMspf()));
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
		[e3, this, i, g]()
	{
		static bool visible = false;

		if (i->IsKeyPushed(VK_F3))
		{
			visible = (visible) ? false : true;
			_controller->ToggleVisible(e3, visible);
		}
		if (visible)
			_controller->Text()->ChangeText(e3, "Average time per frame\n" + g->GetAVGTPFTimes());
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
		[this, i]()
	{
		static bool visible = false;

		if (i->IsKeyPushed(VK_F4))
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
	//_player->AddPower(testPower);
	//Power* testPower2 = new LockOnStrike(_builder, _player->GetEntity(), _AI->GetEnemyList());
	//_player->AddPower(testPower2);

	_allPowers.push_back(new LockOnStrike(_builder, _player->GetEntity(), _AI->GetEnemyList()));
	_allPowers.push_back(new RandomBlink(_builder, _player->GetEntity(), _dungeon->GetFreePositions()));

	size_t firstPower = rand() % _allPowers.size();
	size_t secondPower = (firstPower + 1) % _allPowers.size();

	auto options = System::GetOptions();
	float pctX = options->GetScreenResolutionWidth() / 100.0f;
	float pctY = options->GetScreenResolutionHeight() / 100.0f;
	float midX = options->GetScreenResolutionWidth() / 2.0f;
	float midY = options->GetScreenResolutionHeight() / 2.0f;


	_choice2 = _builder->CreateOverlay(XMFLOAT3(midX - 30.0f * pctX, midY - 20.0f * pctY, 0.0f), 28.0f * pctX, 40.0f * pctY, "Assets/Textures/menuthing.png");
	_choice1 = _builder->CreateOverlay(XMFLOAT3(midX + 2.0f * pctX, midY - 20.0f * pctY, 0.0f), 28.0f * pctX, 40.0f * pctY, "Assets/Textures/menuthing.png");
	_choice2Text = _builder->CreateLabel(XMFLOAT3(midX + 4.0f * pctX, midY - 18.0f * pctY, 0.0f), _allPowers[firstPower]->GetDescription(40), XMFLOAT4(0.8f, 0.8f, 0.5f, 1.0f), 1.0f, 1.0f, "");
	_choice1Text = _builder->CreateLabel(XMFLOAT3(midX - 29.0f * pctX, midY - 18.0f * pctY, 0.0f), _allPowers[secondPower]->GetDescription(40), XMFLOAT4(0.8f, 0.8f, 0.5f, 1.0f), 1.0f, 1.0f, "");
	
	

	_powerLabel = _builder->CreateLabel(XMFLOAT3(midX - 30.0f * pctX, midY - 10.0f * pctY - 20.0f * pctY, 0.0f), "Choose your powers", XMFLOAT4(0.8f, 0.8f, 0.4f, 1.0f), 60.0f * pctX, 8.0f * pctY,"");
	
	_controller->Text()->ChangeFontSize(_powerLabel, 20);
	_controller->Text()->ChangeFontSize(_choice1Text, 20);
	_controller->Text()->ChangeFontSize(_choice2Text, 20);
	
	_powerChosen = false;
	_builder->Event()->BindEvent(_choice1, EventManager::EventType::LeftClick, [this,i,firstPower]() {
		_player->AddPower(_allPowers[firstPower]);
		i->LockMouseToCenter(true);
		_controller->ReleaseEntity(_choice1);
		_controller->ReleaseEntity(_choice2);
		_controller->ReleaseEntity(_powerLabel);
		_controller->ReleaseEntity(_choice1Text);
		_controller->ReleaseEntity(_choice2Text);
		i->HideCursor(true);
	});
	_builder->Event()->BindEvent(_choice2, EventManager::EventType::LeftClick, [this,i,secondPower]() {
		_player->AddPower(_allPowers[secondPower]);
		i->LockMouseToCenter(true);
		i->LockMouseToWindow(true);
		_controller->ReleaseEntity(_choice1);
		_controller->ReleaseEntity(_choice2);
		_controller->ReleaseEntity(_powerLabel);
		_controller->ReleaseEntity(_choice1Text);
		_controller->ReleaseEntity(_choice2Text);
		i->HideCursor(true);
	});
	


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
	_ctimer.TimeStart("State Update");
	State::Update();
	_ctimer.TimeEnd("State Update");
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


	const std::vector<Entity>& ents = _dungeon->GetWalls();
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
			p->CollideWithEntity(outMTV, entity);
		});
	}
	_AI->CheckCollisionAgainstProjectiles(ps);
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
	text += "\nState Update: " + to_string(_ctimer.GetAVGTPF("State Update"));
	text += "\nPlayer Input: " + to_string(_ctimer.GetAVGTPF("Player input"));
	text += "\nCollision world: " + to_string(_ctimer.GetAVGTPF("Collision world"));
	text += "\nPlayer update: " + to_string(_ctimer.GetAVGTPF("Player update"));
	text += "\nAI: " + to_string(_ctimer.GetAVGTPF("AI"));
	text += "\nCulling: " + to_string(_ctimer.GetAVGTPF("Culling"));

	static uint framecount2 = 60;
	framecount2++;
	if (framecount2 > 60)
	{
		_controller->Text()->ChangeText(e4, text);
		framecount2 = 0;
	}
	
	_timeSinceLastSound += _gameTimer.DeltaTime();
}

void GameState::Render()
{
	System::GetGraphics()->Render(_gameTimer.TotalTime(), _gameTimer.DeltaTime());
}
