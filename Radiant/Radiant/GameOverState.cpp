#include "GameOverState.h"
#include "System.h"


GameOverState::GameOverState() : State()
{
	_thePlayer = nullptr;
}

GameOverState::GameOverState(Player* theLoser)
{
	_thePlayer = theLoser;
}

GameOverState::~GameOverState()
{

}

void GameOverState::Init()
{
	auto o = System::GetOptions();
	float width = (float)o->GetScreenResolutionWidth();
	float height = (float)o->GetScreenResolutionHeight();
	auto i = System::GetInput();
	auto c = _controller;
	auto a = System::GetInstance()->GetAudio();
	a->PlayBGMusic(L"mamb.wav", 0.5f);
	XMFLOAT4 TextColor = XMFLOAT4(41.0f / 255.0f, 127.0f / 255.0f, 185.0f / 255.0f, 1.0f);
	XMFLOAT4 ScoreTextColor = XMFLOAT4(17.0f / 255.0f, 166.0f / 255.0f, 67.0f / 255.0f, 1.0f);

	//==========================
	//====	High Score Text	====
	//==========================

	float widthPercentOfDefault = (1.0f / 1920.0f) * width;
	float heightPercentOfDefault = (1.0f / 1080.0f) * height;

	Entity scoreOverlay = _builder->CreateOverlay(XMFLOAT3(0.0f, 0.0f, 0.0f), 550.0f*widthPercentOfDefault, 350.0f*heightPercentOfDefault, "Assets/Textures/GameOverOverlay.png");

	float fontSize = 40 * widthPercentOfDefault;

	//High Score
	Entity textHighScore = _builder->CreateLabel(XMFLOAT3(0.0f, 0.0f, 0.0f),
		"Statistics",
		ScoreTextColor,
		125.0f,
		25.0f,
		"");
	_builder->Text()->ChangeFontSize(textHighScore, (uint)(fontSize*1.5f));
	_builder->Transform()->SetPosition(textHighScore, XMFLOAT3(550.0f*widthPercentOfDefault / 2 - _builder->Text()->GetLength(textHighScore) / 2, 0.0f, 0.0f));
	_builder->Transform()->BindChild(scoreOverlay, textHighScore);

	//Light High Score
	Entity totalLightHighScoreText = _builder->CreateLabel(XMFLOAT3(0.0f, 0.0f, 0.0f),
		"Light Collected:",
		ScoreTextColor,
		125.0f,
		25.0f,
		"");
	_builder->Text()->ChangeFontSize(totalLightHighScoreText, (uint)(fontSize*0.55f));
	_builder->Transform()->SetPosition(totalLightHighScoreText, XMFLOAT3(0.0f, fontSize*1.5f + fontSize * 0 + 10.0f * 1 * heightPercentOfDefault, 0.0f));
	_builder->Transform()->BindChild(scoreOverlay, totalLightHighScoreText);

	Entity totalLightCollectedText = _builder->CreateLabel(XMFLOAT3(0.0f, 0.0f, 0.0f),
		std::to_string(_thePlayer->GetTotalLightCollected()),
		ScoreTextColor,
		125.0f,
		25.0f,
		"");
	_builder->Text()->ChangeFontSize(totalLightCollectedText, (uint)(fontSize*0.55f));
	_builder->Transform()->SetPosition(totalLightCollectedText, XMFLOAT3(560.0f*widthPercentOfDefault / 2, fontSize*1.5f + fontSize * 0 + 10.0f * 1 * heightPercentOfDefault, 0.0f));
	_builder->Transform()->BindChild(scoreOverlay, totalLightCollectedText);


	//Shots Fired
	Entity totalShotsFiredHighScoreText = _builder->CreateLabel(XMFLOAT3(0.0f, 0.0f, 0.0f),
		"Shots Fired:",
		ScoreTextColor,
		125.0f,
		25.0f,
		"");
	_builder->Text()->ChangeFontSize(totalShotsFiredHighScoreText, (uint)(fontSize*0.55f));
	_builder->Transform()->SetPosition(totalShotsFiredHighScoreText, XMFLOAT3(0.0f, fontSize*1.5f + fontSize * 1 + 10.0f * 2 * heightPercentOfDefault, 0.0f));
	_builder->Transform()->BindChild(scoreOverlay, totalShotsFiredHighScoreText);

	Entity totalShotsFiredText = _builder->CreateLabel(XMFLOAT3(0.0f, 0.0f, 0.0f),
		std::to_string(_thePlayer->GetShotsFired()),
		ScoreTextColor,
		125.0f,
		25.0f,
		"");
	_builder->Text()->ChangeFontSize(totalShotsFiredText, (uint)(fontSize*0.55f));
	_builder->Transform()->SetPosition(totalShotsFiredText, XMFLOAT3(560.0f*widthPercentOfDefault / 2, fontSize*1.5f + fontSize * 1 + 10.0f * 2 * heightPercentOfDefault, 0.0f));
	_builder->Transform()->BindChild(scoreOverlay, totalShotsFiredText);


	//Shots Connected
	Entity totalShotsConnectedHighScoreText = _builder->CreateLabel(XMFLOAT3(0.0f, 0.0f, 0.0f),
		"Shots Hitting:",
		ScoreTextColor,
		125.0f,
		25.0f,
		"");
	_builder->Text()->ChangeFontSize(totalShotsConnectedHighScoreText, (uint)(fontSize*0.55f));
	_builder->Transform()->SetPosition(totalShotsConnectedHighScoreText, XMFLOAT3(0.0f, fontSize*1.5f + fontSize * 2 + 10.0f * 3 * heightPercentOfDefault, 0.0f));
	_builder->Transform()->BindChild(scoreOverlay, totalShotsConnectedHighScoreText);

	Entity totalShotsConnectedText = _builder->CreateLabel(XMFLOAT3(0.0f, 0.0f, 0.0f),
		std::to_string(_thePlayer->GetShotsConnected()),
		ScoreTextColor,
		125.0f,
		25.0f,
		"");
	_builder->Text()->ChangeFontSize(totalShotsConnectedText, (uint)(fontSize*0.55f));
	_builder->Transform()->SetPosition(totalShotsConnectedText, XMFLOAT3(560.0f*widthPercentOfDefault / 2, fontSize*1.5f + fontSize * 2 + 10.0f * 3 * heightPercentOfDefault, 0.0f));
	_builder->Transform()->BindChild(scoreOverlay, totalShotsConnectedText);


	//Hit Chance
	Entity totalHitChanceHighScoreText = _builder->CreateLabel(XMFLOAT3(0.0f, 0.0f, 0.0f),
		"Hit Percent:",
		ScoreTextColor,
		125.0f,
		25.0f,
		"");
	_builder->Text()->ChangeFontSize(totalHitChanceHighScoreText, (uint)(fontSize*0.55f));
	_builder->Transform()->SetPosition(totalHitChanceHighScoreText, XMFLOAT3(0.0f, fontSize*1.5f + fontSize * 3 + 10.0f * 4 * heightPercentOfDefault, 0.0f));
	_builder->Transform()->BindChild(scoreOverlay, totalHitChanceHighScoreText);
	float value = _thePlayer->GetHitPercent();
	int intValue = static_cast<int>(value);
	Entity totalHitPercentText = _builder->CreateLabel(XMFLOAT3(0.0f, 0.0f, 0.0f),
		std::to_string(intValue) + "." + std::to_string(static_cast<int>((value-(float)intValue)*100))+ "%",
		ScoreTextColor,
		125.0f,
		25.0f,
		"");
	_builder->Text()->ChangeFontSize(totalHitPercentText, (uint)(fontSize*0.55f));
	_builder->Transform()->SetPosition(totalHitPercentText, XMFLOAT3(560.0f*widthPercentOfDefault / 2, fontSize*1.5f + fontSize * 3 + 10.0f * 4 * heightPercentOfDefault, 0.0f));
	_builder->Transform()->BindChild(scoreOverlay, totalHitPercentText);


	//Enemies Defeated
	Entity totalEnemiesDefeatedScoreText = _builder->CreateLabel(XMFLOAT3(0.0f, 0.0f, 0.0f),
		"Enemies Defeated:",
		ScoreTextColor,
		125.0f,
		25.0f,
		"");
	_builder->Text()->ChangeFontSize(totalEnemiesDefeatedScoreText, (uint)(fontSize*0.55f));
	_builder->Transform()->SetPosition(totalEnemiesDefeatedScoreText, XMFLOAT3(0.0f, fontSize*1.5f + fontSize * 4 + 10.0f * 5 * heightPercentOfDefault, 0.0f));
	_builder->Transform()->BindChild(scoreOverlay, totalEnemiesDefeatedScoreText);

	Entity totalEnemiesDefeatedText = _builder->CreateLabel(XMFLOAT3(0.0f, 0.0f, 0.0f),
		std::to_string(_thePlayer->GetEnemiesDefeated()),
		ScoreTextColor,
		125.0f,
		25.0f,
		"");
	_builder->Text()->ChangeFontSize(totalEnemiesDefeatedText, (uint)(fontSize*0.75f));
	_builder->Transform()->SetPosition(totalEnemiesDefeatedText, XMFLOAT3(560.0f*widthPercentOfDefault / 2, fontSize*1.5f + fontSize * 4 + 10.0f * 5 * heightPercentOfDefault, 0.0f));
	_builder->Transform()->BindChild(scoreOverlay, totalEnemiesDefeatedText);



	//Set the overlay, and all it's childrens', positions.
	_builder->Transform()->SetPosition(scoreOverlay, XMFLOAT3(width - 550.0f*widthPercentOfDefault, height - 350.0f*heightPercentOfDefault, 0.0f));


	//==========================
	//====		Buttons		====
	//==========================


	// Start game button
	Entity b1 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 230.0f, 0.0f),
		"New Game",
		TextColor,
		250.0f,
		45.0f,
		"",
		[i, a]()
	{
		a->PlaySoundEffect(L"menuclick.wav", 1);
		ChangeStateTo(StateChange(new GameState()));
	});

	//Options button
	Entity b5 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 180.0f, 0.0f),
		"Main Menu",
		TextColor,
		250.0f,
		45.0f,
		"",
		[i, a]()
	{
		a->PlaySoundEffect(L"menuclick.wav", 1);
		ChangeStateTo(StateChange(new MenuState));
	});


	// Exit button
	Entity b2 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 130.0f, 0.0f),
		"Exit",
		TextColor,
		250.0f,
		45.0f,
		"",
		[a]() {
		a->PlaySoundEffect(L"menuclick.wav", 1);
		ExitApplication;
	});

	//==========================
	//====	Background		====
	//==========================

	// Game Over text
	_builder->CreateLabel(
		XMFLOAT3(width / 2.0f - 100.0f, 25.0f, 0.0f),
		"Game Over",
		TextColor,
		250.0f,
		45.0f,
		"");

	_altar = _builder->CreateObject(
		XMVectorSet(1.5f, 0.0f, 1.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.5f, 0.5f, 0.5f, 0.0f),
		"Assets/Models/Altar.arf",
		"Assets/Textures/Altar_Albedo.png",
		"Assets/Textures/Altar_NM.png",
		"Assets/Textures/Floor_Disp.png",
		"Assets/Textures/Altar_Roughness.png");
	_builder->Material()->SetMaterialProperty(_altar, "ParallaxBias", -0.05f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_altar, "ParallaxScaling", 0.12f, "Shaders/GBuffer.hlsl");


	_builder->Bounding()->CreateBoundingSphere(_altar, 2.0f);

	_altarCenterLight = _builder->EntityC().Create();
	_builder->Light()->BindPointLight(_altarCenterLight, XMFLOAT3(0.0f, 0.0f, 0.0f), 3.0f, XMFLOAT3(0.0f, 0.25f, 0.35f), 10.0f);
	_builder->Light()->ChangeLightBlobRange(_altarCenterLight, 1.0f);
	_builder->Transform()->CreateTransform(_altarCenterLight);
	_builder->Transform()->SetPosition(_altarCenterLight, XMFLOAT3(0.0f, 1.4f, 0.0f));
	_builder->Transform()->BindChild(_altar, _altarCenterLight);

	for (int i = 0; i < _numAltarBolts; ++i)
	{
		_altarBolts[i] = _builder->EntityC().Create();
		_builder->Light()->BindPointLight(_altarBolts[i], XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f, XMFLOAT3(0.35f, 1.0f, 0.25f), 5.0f);
		_builder->Light()->ChangeLightBlobRange(_altarBolts[i], 0.3f);
		_builder->Lightning()->CreateLightningBolt(_altarBolts[i], _altarCenterLight);
		_builder->Transform()->CreateTransform(_altarBolts[i]);
		_builder->Transform()->BindChild(_altarCenterLight, _altarBolts[i]);

		float angle = XM_2PI / _numAltarBolts;
		_builder->Transform()->SetPosition(_altarBolts[i], XMFLOAT3(1.5f * sinf(i * angle), 0.0f, 1.5f * cosf(i * angle)));
		_builder->Transform()->SetScale(_altarBolts[i], XMVectorSet(0.4f, 0.4f, 1.0f, 1.0f));

		_altarBoltAngle[i] = i * angle;
	}

	_builder->Transform()->SetPosition(_altar, XMFLOAT3(0.0f, 0.0f, 0.0f));


	_controller->BindEventHandler(_altar, EventManager::Type::Object);
	_controller->BindEvent(_altar, EventManager::EventType::Update,
		[this]()
	{
		_builder->Transform()->RotateYaw(_altarCenterLight, _gameTimer.DeltaTime() * 25.0f);
		static float animDeltaTime = 0;
		animDeltaTime += _gameTimer.DeltaTime();
		bool resetAnimTime = false;
		for (int i = 0; i < _numAltarBolts; ++i)
		{
			_altarBoltAngle[i] += _gameTimer.DeltaTime() * XM_PIDIV2 + _gameTimer.DeltaTime()*XM_PI * (i / 6);
			if (_altarBoltAngle[i] >= XM_2PI)
				_altarBoltAngle[i] -= XM_2PI;

			XMVECTOR pos = _builder->Transform()->GetPosition(_altarBolts[i]);
			_builder->Transform()->SetPosition(_altarBolts[i], XMVectorSetY(pos, 0.8f * sinf(_altarBoltAngle[i])));

			if (animDeltaTime >= 0.02f)
			{
				resetAnimTime = true;
				_builder->Lightning()->Animate(_altarBolts[i]);
			}
		}

		if (resetAnimTime)
			animDeltaTime -= 0.02f;

	});

	Entity camera = _builder->CreateCamera(XMVectorSet(0.0f, 1.0f, -3.0f, 0.0f));
	_builder->Transform()->SetRotation(camera, XMFLOAT3(0.0f, 0.0f, 0.0f));



	//==========================
	//====		Update		====
	//==========================

	_controller->BindEvent(b2, EventManager::EventType::Update, [i, this]()
	{
		if (i->IsKeyPushed(VK_ESCAPE))
		{
			ExitApplication;
		}
		if (i->IsKeyPushed('1'))
		{
			ChangeStateTo(StateChange(new GameState()));
		}
		if (i->IsKeyPushed('2'))
		{
			ChangeStateTo(StateChange(new MenuState()));
		}
	}
	);

}

void GameOverState::Shutdown()
{
	SAFE_SHUTDOWN(_savedState);
}

