#include "GameState.h"
#include "System.h"
#include "Graphics.h"
#include "PowersHeader.h"
#include "ChoosePowerState.h"

using namespace DirectX;
#define SizeOfSide 50
#define NrOfEnemiesAtStart 20
#define difficultySteps 0.25f
#define levelDifficultyIncrease 0.05f
#define nrOfWeaponsToSpawn 10

GameState::GameState() : State(),_lightRemaning(0.0f), _lightTreshold(0.0f), _timeSinceLastSound(0.0f), _currentPreQuoteSound(0), _currentAfterQuoteSound(0), e4(Entity()), _altar(Entity()), _quadTree(Entity())
{
	_currentLevel = 6; //4 = all weapons/enemies. Change to lower before "release"
}

GameState::GameState(Player * player, int lastLevel) :State(), _lightRemaning(0.0f), _lightTreshold(0.0f), _timeSinceLastSound(0.0f), _currentPreQuoteSound(0), _currentAfterQuoteSound(0), e4(Entity()), _altar(Entity()), _quadTree(Entity())
{
	//Something for the player and creating a new state. Or something? God, have fun figured out that with Init.
	_currentLevel = lastLevel + 1;
}

GameState::~GameState()
{
}

void GameState::Init()
{
	_timeSinceLastSound = 100;
	_currentPreQuoteSound = 0;
	_currentAfterQuoteSound = 0;

	auto i = System::GetInput();
	auto o = System::GetOptions();
	float width = (float)o->GetScreenResolutionWidth();
	float height = (float)o->GetScreenResolutionHeight();
	auto c = _controller;
	auto a = System::GetInstance()->GetAudio();
	float widthPercentOfDefault = (1.0f / 1920.0f) * width;
	float heightPercentOfDefault = (1.0f / 1080.0f) * height;
	float fontSize = 40 * widthPercentOfDefault;

	XMFLOAT4 TextColor = XMFLOAT4(41.0f / 255.0f, 127.0f / 255.0f, 185.0f / 255.0f, 1.0f);

	//==================================
	//====	Create All Things		====
	//==================================
	try { _player = new Player(_builder); }
	catch (std::exception& e) { e; throw ErrorMsg(3000005, L"Failed to create a player in the GameState."); }

	//==================================
	//====	Give me zee dungeon		====
	//==================================
	_dungeon = new Dungeon(SizeOfSide, 5, 6, 0.85f, _builder, _currentLevel);

	auto positions = _dungeon->GetFreeRoomPositions();

	//==================================
	//====		Set Camera			====
	//==================================
	_player->SetCamera();

	FreePositions p = positions[rand() % positions.size()];
	//FreePositions p = positions[rand() % positions.size()];
	_altar = _builder->CreateObject(
		XMVectorSet((float)p.x, 0.0f, (float)p.y,1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f,0.0f),
		XMVectorSet(0.5f, 0.5f, 0.5f, 0.0f),
		"Assets/Models/Altar.arf",
		"Assets/Textures/Altar_Albedo.png",
		"Assets/Textures/Altar_NM.png",
		"Assets/Textures/default_displacement.png",
		"Assets/Textures/Altar_Roughness.png");
	_builder->Material()->SetMaterialProperty(_altar, "ParallaxBias", -0.05f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_altar, "ParallaxScaling", 0.12f, "Shaders/GBuffer.hlsl");


	_builder->Bounding()->CreateBoundingSphere(_altar, 2.0f);



	_altarCenterLight = _builder->EntityC().Create();
	_builder->Light()->BindPointLight( _altarCenterLight, XMFLOAT3(0.0f, 0.0f, 0.0f), 3.0f, XMFLOAT3(1.0f, 1.0f, 1.0f), 10.0f);
	_builder->Light()->ChangeLightBlobRange( _altarCenterLight, 1.0f);
	_builder->Transform()->CreateTransform( _altarCenterLight );
	_builder->Transform()->SetPosition( _altarCenterLight, XMFLOAT3(0.0f, 1.4f, 0.0f));
	_builder->Transform()->BindChild(_altar, _altarCenterLight );

	for ( int i = 0; i < _numAltarBolts; ++i )
	{
		_altarBolts[i] = _builder->EntityC().Create();
		_builder->Light()->BindPointLight( _altarBolts[i], XMFLOAT3( 0.0f, 0.0f, 0.0f ), 1.0f, XMFLOAT3( 1.0f, 1.0f, 1.0f ), 5.0f );
		_builder->Light()->ChangeLightBlobRange( _altarBolts[i], 0.3f );
		_builder->Lightning()->CreateLightningBolt( _altarBolts[i], _altarCenterLight );
		_builder->Transform()->CreateTransform( _altarBolts[i] );
		_builder->Transform()->BindChild( _altarCenterLight, _altarBolts[i] );

		float angle = XM_2PI / _numAltarBolts;
		_builder->Transform()->SetPosition( _altarBolts[i], XMFLOAT3( 1.5f * sinf( i * angle ), 0.0f, 1.5f * cosf( i * angle ) ) );
		_builder->Transform()->SetScale( _altarBolts[i], XMVectorSet( 0.4f, 0.4f, 1.0f, 1.0f ) );

		_altarBoltAngle[i] = i * angle;
	}

	_builder->Transform()->SetPosition(_altar, XMFLOAT3((float)p.x, 0.0f, (float)p.y));

	Entity ndl = _builder->CreateLabel(
		XMFLOAT3(width/2.0f - 300.0f, height /2.0f - 50.0f, 0.0f),
		"You must collect more light!",
		fontSize,
		TextColor,
		300.0f,
		50.0f,
		"");
	_controller->ToggleVisible(ndl, false);
	Entity bdone = _builder->CreateButton(
		XMFLOAT3(System::GetOptions()->GetScreenResolutionWidth() / 2.0f - 75.0f, System::GetOptions()->GetScreenResolutionHeight() / 2.0f - 50.0f, 0.0f),
		"Proceed",
		fontSize,
		TextColor,
		250.0f,
		50.0f,
		"",
		[i, a,this]()
	{
		a->PlaySoundEffect(L"menuclick.wav", 1);
		i->LockMouseToCenter(true);
		i->LockMouseToWindow(true);
		i->HideCursor(true);
		ChangeStateTo(StateChange(new ChoosePowerState(), true, false, false));
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

	//_controller->Light()->ChangeLightRange(_player->GetEntity(), (1.2f - _AI->GetLightPoolPercent())*10.0);
	//_controller->Camera()->SetDrawDistance(_player->GetEntity(), 35);
	p = _dungeon->GetunoccupiedSpace();


	//Set the player to the first "empty" space we find in the map, +0.5 in x and z

	_player->SetPosition(XMVectorSet((float)p.x, 0.5f, (float)p.y, 1.0f));

	//==================================
	//====	Give me zee AI			====
	//==================================
	_AI = new Shodan(_builder, _dungeon, SizeOfSide, _player);


	//==================================
	//====	Level Specifics			====
	//==================================
	switch (_currentLevel)
	{
	case 1:
	{
		//Enemies to spawn
		EnemyTypes enemyTypes[1];
		enemyTypes[0] = EnemyTypes::ENEMY_TYPE_NORMAL;
		_AI->AddEnemyStartOfLevel(enemyTypes, 1, NrOfEnemiesAtStart);
		break;
	}
	case 2:
	{
		EnemyTypes enemyTypes[2];
		enemyTypes[0] = EnemyTypes::ENEMY_TYPE_NORMAL;
		enemyTypes[1] = EnemyTypes::ENEMY_TYPE_TELEPORTER;
		_AI->AddEnemyStartOfLevel(enemyTypes, 2, NrOfEnemiesAtStart);

		_CreateWeapons(Weapons::RapidFire, nrOfWeaponsToSpawn);
		break;
	}
	case 3:
	{
		EnemyTypes enemyTypes[3];
		enemyTypes[0] = EnemyTypes::ENEMY_TYPE_NORMAL;
		enemyTypes[1] = EnemyTypes::ENEMY_TYPE_TELEPORTER;
		enemyTypes[2] = EnemyTypes::ENEMY_TYPE_MINI_GUN;
		_AI->AddEnemyStartOfLevel(enemyTypes, 3, NrOfEnemiesAtStart);

		_CreateWeapons(Weapons::RapidFire | Weapons::Shotgun, nrOfWeaponsToSpawn);
		break;
	}
	default:
	{
		//Spawning Enemies
		_AI->AddEnemyStartOfLevel(NrOfEnemiesAtStart);

		_CreateWeapons( Weapons::Charge | Weapons::Bounce | Weapons::FragBomb | Weapons::LightThrower | Weapons::RapidFire | Weapons::Rocket | Weapons::Shotgun, nrOfWeaponsToSpawn);
		break;
	}
	}


	Difficulty thisDifficulty = Difficulty::NORMAL_DIFFICULTY;
	switch (System::GetOptions()->GetDifficulty())
	{
	case 0:
	{
		thisDifficulty = Difficulty::EASY_DIFFICULTY;
		break;
	}
	case 1:
	{
		break;
	}
	case 2:
	{
		thisDifficulty = Difficulty::HARD_DIFFICULTY;
		break;
	}
	case 3:
	{
		thisDifficulty = Difficulty::WHY_DID_YOU_CHOOSE_THIS_DIFFICULTY;
		break;
	}
	}

	switch (thisDifficulty)
	{
	case Difficulty::EASY_DIFFICULTY:
	{
		_AI->SetDifficultyBonus(1.0f + _currentLevel*levelDifficultyIncrease - difficultySteps);
		break;
	}
	case Difficulty::HARD_DIFFICULTY:
	{
		_AI->SetDifficultyBonus(1.0f + _currentLevel*levelDifficultyIncrease + difficultySteps);
		break;
	}
	case Difficulty::WHY_DID_YOU_CHOOSE_THIS_DIFFICULTY:
	{
		_AI->SetDifficultyBonus(1.0f + _currentLevel*levelDifficultyIncrease + 5 * difficultySteps);
		break;
	}
	default:
	{
		_AI->SetDifficultyBonus(1.0f + _currentLevel*levelDifficultyIncrease);
		break;
	}
	}

	_quadTree = _builder->EntityC().Create();
	const std::vector<Entity>& walls = _dungeon->GetWalls();
	const std::vector<Entity>& fr = _dungeon->GetFloorRoof();
	const std::vector<Entity>& pillars = _dungeon->GetPillars();

	std::vector<Entity> vect;
	vect.insert(vect.begin(), walls.begin(), walls.end());
	vect.insert(vect.begin(), fr.begin(), fr.end());
	vect.insert(vect.begin(), pillars.begin(), pillars.end());
	

	_builder->Bounding()->CreateQuadTree(_quadTree, vect);

	//==================================
	//====		Set Input data		====
	//==================================


	Entity e = _builder->CreateLabel(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		"FPS: 0",
		fontSize,
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
		fontSize,
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
		fontSize,
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
		fontSize,
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



	/*Power* testPower = new RandomBlink(_builder, _player->GetEntity(), _dungeon->GetFreePositions());
	_player->AddPower(testPower);
	Power* testPower2 = new LockOnStrike(_builder, _player->GetEntity(), _AI->GetEnemyList());
	_player->AddPower(testPower2);
	Power* testPower3 = new CharmPower(_builder, _player->GetEntity(), _AI->GetEnemyList());
	_player->AddPower(testPower3);
	Power* testPower4 = new TimeStopper(_builder, _player->GetEntity(), _AI->GetEnemyList());
	_player->AddPower(testPower4);*/
	
	i->LockMouseToCenter(true);
	i->LockMouseToWindow(true);
	i->HideCursor(true);

	

	

}

void GameState::Shutdown()
{
	State::Shutdown();

	SAFE_DELETE(_player);
	SAFE_DELETE(_dungeon);
	SAFE_DELETE(_AI);
	SAFE_DELETE(_clutter);
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
		ChangeStateTo(StateChange(new PauseState,true,false,true));
	}
	_ctimer.TimeStart("Player input");
	_player->HandleInput(_gameTimer.DeltaTime());
	_ctimer.TimeEnd("Player input");

	// Rotate the center altar light, causing the bound children to follow
	_builder->Transform()->RotateYaw( _altarCenterLight, _gameTimer.DeltaTime() * 25.0f );

	// Let the child lights of the altar wobble up and down
	static float animDeltaTime = 0;
	animDeltaTime += _gameTimer.DeltaTime();
	bool resetAnimTime = false;
	for ( int i = 0; i < _numAltarBolts; ++i )
	{
		_altarBoltAngle[i] += _gameTimer.DeltaTime() * XM_PIDIV2;
		if ( _altarBoltAngle[i] >= XM_2PI )
			_altarBoltAngle[i] -= XM_2PI;

		XMVECTOR pos = _builder->Transform()->GetPosition( _altarBolts[i] );
		_builder->Transform()->SetPosition(_altarBolts[i], XMVectorSetY( pos, 0.8f * sinf( _altarBoltAngle[i] ) ) );

		if ( animDeltaTime >= 0.05f )
		{
			resetAnimTime = true;
			_builder->Lightning()->Animate( _altarBolts[i] );
		}
	}

	if ( resetAnimTime )
		animDeltaTime -= 0.05f;

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
	static float prev2 = _player->GetHealthPercent();
	static float curr2 = prev2;
	XMFLOAT3 sColor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 eColor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	curr2 = _player->GetHealthPercent();
	if (curr2 < prev2)
	{
		float delta = prev2 - curr2;
		prev2 -= _gameTimer.DeltaTime()*delta+0.1;
		if (prev2 < curr2)
			prev2 = curr2;
		for (auto& e : ents)
		{
			XMFLOAT3 color = XMFLOAT3(sColor.x*prev2 + eColor.x*(1.0f - prev2), sColor.y*prev2 + eColor.y*(1.0f - prev2), sColor.z*prev2 + eColor.z*(1.0f - prev2));
			//_controller->Material()->SetMaterialProperty(e, "EmissiveIntensity", prev2 / 100.0f, "Shaders/GBufferEmissive.hlsl");
			_builder->Material()->SetMaterialProperty(e, "EmissiveColor", color, "Shaders/GBufferEmissive.hlsl");
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
		ChangeStateTo(StateChange(new GameOverState(_player),true));
		return;
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

void GameState::ProgressNoNextLevel(unsigned int power)
{
	SAFE_DELETE(_AI);
	SAFE_DELETE(_dungeon);
	_currentLevel++;
	_dungeon = new Dungeon(SizeOfSide, 4, 7, 0.75f, _builder, _currentLevel);

	FreePositions p = _dungeon->GetunoccupiedSpace();
	_builder->Transform()->SetPosition(_altar, XMFLOAT3((float)p.x, 0.0f, (float)p.y));
	_currentPreQuoteSound = 0;

	p = _dungeon->GetunoccupiedSpace();
	_player->SetPosition(XMVectorSet((float)p.x, 0.5f, (float)p.y, 1.0f));
	_player->ResetPlayerForLevel(System::GetOptions()->GetHardcoreMode());

	_AI = new Shodan(_builder, _dungeon, SizeOfSide, _player);


	//==================================
	//====	Level Specifics			====
	//==================================

	switch (_currentLevel)
	{
	case 1:
	{
		//Enemies to spawn
		EnemyTypes enemyTypes[1];
		enemyTypes[0] = EnemyTypes::ENEMY_TYPE_NORMAL;
		_AI->AddEnemyStartOfLevel(enemyTypes, 1, NrOfEnemiesAtStart);
		break;
	}
	case 2:
	{
		EnemyTypes enemyTypes[2];
		enemyTypes[0] = EnemyTypes::ENEMY_TYPE_NORMAL;
		enemyTypes[1] = EnemyTypes::ENEMY_TYPE_TELEPORTER;
		_AI->AddEnemyStartOfLevel(enemyTypes, 2, NrOfEnemiesAtStart);

		_CreateWeapons(Weapons::RapidFire, 5);
		break;
	}
	case 3:
	{
		EnemyTypes enemyTypes[3];
		enemyTypes[0] = EnemyTypes::ENEMY_TYPE_NORMAL;
		enemyTypes[1] = EnemyTypes::ENEMY_TYPE_TELEPORTER;
		enemyTypes[2] = EnemyTypes::ENEMY_TYPE_MINI_GUN;
		_AI->AddEnemyStartOfLevel(enemyTypes, 3, NrOfEnemiesAtStart);

		_CreateWeapons(Weapons::RapidFire | Weapons::Shotgun, 5);
		break;
	}
	case 4:
	{
		EnemyTypes enemyTypes[2];
		enemyTypes[0] = EnemyTypes::ENEMY_TYPE_TELEPORTER;
		enemyTypes[1] = EnemyTypes::ENEMY_TYPE_MINI_GUN;
		_AI->AddEnemyStartOfLevel(enemyTypes, 2, NrOfEnemiesAtStart);

		_CreateWeapons(Weapons::RapidFire | Weapons::Shotgun | Weapons::Bounce | Weapons::LightThrower, nrOfWeaponsToSpawn);
		break;
	}
	case 5:
	{
		EnemyTypes enemyTypes[2];
		enemyTypes[0] = EnemyTypes::ENEMY_TYPE_NORMAL;
		enemyTypes[1] = EnemyTypes::ENEMY_TYPE_MINI_GUN;
		_AI->AddEnemyStartOfLevel(enemyTypes, 2, NrOfEnemiesAtStart);

		_CreateWeapons(Weapons::Charge, 1);
		break;
	}
	case 6:
	{
		EnemyTypes enemyTypes[3];
		enemyTypes[0] = EnemyTypes::ENEMY_TYPE_NORMAL;
		enemyTypes[1] = EnemyTypes::ENEMY_TYPE_TELEPORTER;
		enemyTypes[2] = EnemyTypes::ENEMY_TYPE_MINI_GUN;
		_AI->AddEnemyStartOfLevel(enemyTypes, 3, NrOfEnemiesAtStart);

		_CreateWeapons(Weapons::Rocket | Weapons::FragBomb | Weapons::LightThrower, nrOfWeaponsToSpawn);
		break;
	}
	default:
	{
		//Spawning Enemies
		_AI->AddEnemyStartOfLevel(NrOfEnemiesAtStart);

		_CreateWeapons(Weapons::Charge | Weapons::Bounce | Weapons::FragBomb | Weapons::LightThrower | Weapons::RapidFire| Weapons::Rocket | Weapons::Shotgun, nrOfWeaponsToSpawn);
		break;
	}
	}

	Difficulty thisDifficulty = Difficulty::NORMAL_DIFFICULTY;
	switch (System::GetOptions()->GetDifficulty())
	{
	case 0:
		{	
			thisDifficulty = Difficulty::EASY_DIFFICULTY;
			break;
		}
	case 1:
		{
			break;
		}
	case 2:
		{
			thisDifficulty = Difficulty::HARD_DIFFICULTY;
			break;
		}
	case 3:
		{
			thisDifficulty = Difficulty::WHY_DID_YOU_CHOOSE_THIS_DIFFICULTY;
			break;
		}
	}

	switch (thisDifficulty)
	{
	case Difficulty::EASY_DIFFICULTY:
	{
		_AI->SetDifficultyBonus(1.0f + _currentLevel*levelDifficultyIncrease - difficultySteps);
		break;
	}
	case Difficulty::HARD_DIFFICULTY:
	{
		_AI->SetDifficultyBonus(1.0f + _currentLevel*levelDifficultyIncrease + difficultySteps);
		break;
	}
	case Difficulty::WHY_DID_YOU_CHOOSE_THIS_DIFFICULTY:
	{
		_AI->SetDifficultyBonus(1.0f + _currentLevel*levelDifficultyIncrease + 5 * difficultySteps);
		break;
	}
	default:
	{
		_AI->SetDifficultyBonus(1.0f + _currentLevel*levelDifficultyIncrease);
		break;
	}
	}

	_controller->ReleaseEntity(_quadTree);
	const std::vector<Entity>& walls = _dungeon->GetWalls();
	const std::vector<Entity>& fr = _dungeon->GetFloorRoof();

	std::vector<Entity> vect;
	vect.insert(vect.begin(), walls.begin(), walls.end());
	vect.insert(vect.begin(), fr.begin(), fr.end());

	_builder->Bounding()->CreateQuadTree(_quadTree, vect);

	std::vector<power_id_t> powInfo;
	RandomBlink* randomBlink = new RandomBlink(_builder, _player->GetEntity(), _dungeon->GetFreePositions());
	LockOnStrike* lockOnStrike = new LockOnStrike(_builder, _player->GetEntity(), _AI->GetEnemyList());
	CharmPower* charm = new CharmPower(_builder, _player->GetEntity(), _AI->GetEnemyList());
	TimeStopper* timeStopper = new TimeStopper(_builder, _player->GetEntity(), _AI->GetEnemyList());
	_player->GetPowerInfo(powInfo);
	_player->ClearAllPowers();
	power_id_t powerToGive = static_cast<power_id_t>(power);
	bool blinkAdded = false;
	bool lockOnAdded = false;
	bool charmAdded = false;
	bool timeStopperAdded = false;
	for (auto &i : powInfo)
	{
		if (i == randomBlink->GetType())
		{
			_player->AddPower(randomBlink);
			blinkAdded = true;
		}
		else if (i == lockOnStrike->GetType())
		{
			_player->AddPower(lockOnStrike);
			lockOnAdded = true;
		}
		else if (i == charm->GetType())
		{
			_player->AddPower(charm);
			charmAdded = true;
		}
		else if (i == timeStopper->GetType())
		{
			_player->AddPower(timeStopper);
			timeStopperAdded = true;
		}
	}
	if (powerToGive == power_id_t::RANDOMBLINK)
	{
		_player->AddPower(randomBlink);
		blinkAdded = true;
	}
	if (powerToGive == power_id_t::LOCK_ON_STRIKE)
	{
		_player->AddPower(lockOnStrike);
		lockOnAdded = true;
	}
	if (powerToGive == power_id_t::CHARMPOWER)
	{
		_player->AddPower(charm);
		charmAdded = true;
	}
	if (powerToGive == power_id_t::TIMESTOPPER)
	{
		_player->AddPower(timeStopper);
		timeStopperAdded = true;
	}

	if (!blinkAdded)
	{
		delete randomBlink;
	}
	if (!lockOnAdded)
	{
		delete lockOnStrike;
	}
	if (!charmAdded)
	{
		delete charm;
	}
	if (!timeStopperAdded)
	{
		delete timeStopper;
	}



}

Player * GameState::GetPlayer()
{
	return _player;
}

void GameState::_CreateWeapons(unsigned int types, unsigned int nrofweps)
{
	auto a = System::GetInstance()->GetAudio();

	std::vector<Weapons> weps;
	for (unsigned int i = 0; i < Weapons::Num_Weapons; i++)
	{
		unsigned int type = types & 1 << i;
		if (type)
		{
			weps.push_back(static_cast<Weapons>(type));
		}
	}
	//Spawning Weapons
	for (unsigned int j = 0; j < nrofweps; j++)
	{
		FreePositions p = _dungeon->GetunoccupiedSpace();

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

		int rande = (rand() % (weps.size()*100)) / 100;
		switch (weps[rande])
		{
		case Weapons::Bounce:
		{
			_builder->Material()->SetEntityTexture(wep, "DiffuseMap", L"Assets/Textures/bouncetex.dds");
			_builder->Material()->SetEntityTexture(wep2, "DiffuseMap", L"Assets/Textures/bouncetex.dds");
			break;
		}
		case Weapons::FragBomb:
		{

			_builder->Material()->SetEntityTexture(wep, "DiffuseMap", L"Assets/Textures/fragguntex.dds");
			_builder->Material()->SetEntityTexture(wep2, "DiffuseMap", L"Assets/Textures/fragguntex.dds");
			break;
		}
		case Weapons::RapidFire:
		{


			_builder->Material()->SetEntityTexture(wep, "DiffuseMap", L"Assets/Textures/rapidguntex.dds");
			_builder->Material()->SetEntityTexture(wep2, "DiffuseMap", L"Assets/Textures/rapidguntex.dds");
		}
		break;
		case Weapons::Shotgun:
		{

			_builder->Material()->SetEntityTexture(wep, "DiffuseMap", L"Assets/Textures/shotguntex.dds");
			_builder->Material()->SetEntityTexture(wep2, "DiffuseMap", L"Assets/Textures/shotguntex.dds");
		}
		break;
		default:
			_builder->Material()->SetEntityTexture(wep, "DiffuseMap", L"Assets/Textures/bthcolor.dds");
			_builder->Material()->SetEntityTexture(wep2, "DiffuseMap", L"Assets/Textures/bthcolor.dds");
			break;
		}


		_controller->BindEvent(wep, EventManager::EventType::Update,
			[wep, wep2, wrap, this, weps, rande, a]()
		{

			_controller->Transform()->RotateYaw(wep, _gameTimer.DeltaTime() * 50);
			_controller->Transform()->RotateYaw(wep2, _gameTimer.DeltaTime() * -50);
			_controller->Transform()->RotatePitch(wep2, _gameTimer.DeltaTime() * -50);
			if (_controller->Bounding()->CheckCollision(_player->GetEntity(), wrap) != 0) // TEST
			{
				a->PlaySoundEffect(L"weppickup.wav", 1.0f);
				_player->AddWeapon(weps[rande]);

				_controller->ReleaseEntity(wep);
				_controller->ReleaseEntity(wep2);
				_controller->ReleaseEntity(wrap);
			}
		});
	}
}
