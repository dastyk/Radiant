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
	SAFE_DELETE(_thePlayer)
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

	//New Game Plus?
	Entity b3 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 180.0f, 0.0f),
		"Cheat Mode",
		TextColor,
		250.0f,
		45.0f,
		"",
		[i, a, this]()
	{
		a->PlaySoundEffect(L"menuclick.wav", 1);
		Player* tempPlayer = this->_thePlayer;
		this->_thePlayer = nullptr;
		ChangeStateTo(StateChange(new GameState(tempPlayer, -1)));
	});


	//Options button
	Entity b5 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 130.0f, 0.0f),
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
		XMFLOAT3(50.0f, height - 80.0f, 0.0f),
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

	Entity light1 = _builder->EntityC().Create();
	_builder->Light()->BindPointLight(light1, XMFLOAT3(1.5f, 1.0f, 1.0f), 3.0f, XMFLOAT3(1.0f, 1.0f, 1.0f), 15.0f);
	_builder->Light()->ChangeLightBlobRange(light1, 0.5f);
	_builder->Transform()->CreateTransform(light1);
	_builder->Transform()->SetPosition(light1, XMFLOAT3(2.0f, 0.1f, 0.0f));
	_controller->BindEventHandler(light1, EventManager::Type::Object);
	_controller->BindEvent(light1, EventManager::EventType::Update,
		[this, light1]()
	{
		_controller->Transform()->RotateYaw(light1, -45.0f*_gameTimer.DeltaTime());
		_controller->Transform()->MoveForward(light1, 0.5f*_gameTimer.DeltaTime());

	});

	Entity camera = _builder->CreateCamera(XMVectorSet(0.0f, 0.0f, -2.0f, 0.0f));
	_builder->Transform()->SetRotation(camera, XMFLOAT3(15.0f, 0.0f, 0.0f));



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
			ChangeStateTo(StateChange(new GameState(_thePlayer, -1)));
		}
		if (i->IsKeyPushed('3'))
		{
			ChangeStateTo(StateChange(new MenuState()));
		}
	}
	);

}

void GameOverState::Shutdown()
{
}

