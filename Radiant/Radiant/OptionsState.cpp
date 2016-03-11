#include "OptionsState.h"
#include "System.h"


OptionsState::OptionsState() : State()
{
}


OptionsState::~OptionsState()
{
}

void OptionsState::Init()
{
	XMFLOAT4 TextColor = XMFLOAT4(41.0f / 255.0f, 127.0f / 255.0f, 185.0f / 255.0f, 1.0f);


	auto i = System::GetInput();
	auto o = System::GetOptions();
	float width = (float)o->GetScreenResolutionWidth();
	float height = (float)o->GetScreenResolutionHeight();
	auto c = _controller;
	auto a = System::GetInstance()->GetAudio();
	float widthPercentOfDefault = (1.0f / 1920.0f) * width;
	float heightPercentOfDefault = (1.0f / 1080.0f) * height;
	float fontSize = 40 * widthPercentOfDefault;

	float OffsetY = 0.0f;
	float OffsetX = width / 2.0f;

	// Background image
	_builder->CreateImage(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		width,
		height,
		"Assets/Textures/conceptArt.png");

	// Options text
	Entity opt = _builder->CreateLabel(
		XMFLOAT3(OffsetX - 100.0f, 25.0f, 0.0f),
		"Options",
		fontSize,
		TextColor,
		250.0f,
		45.0f,
		"");

	_builder->Text()->ChangeFontSize(opt, (uint)(fontSize));
	//Add the "current" height to OffsetY, then set the current text to this offset in Y. This way, we can change positions on everything without having to worry.
	OffsetY += 25.0f*heightPercentOfDefault;
	_builder->Transform()->SetPosition(opt, XMFLOAT3(OffsetX - _builder->Text()->GetLength(opt)/2.0f, OffsetY, 0.0f));
	// Save Changes button
	Entity b1 = _builder->CreateButton(
		XMFLOAT3(50.0f*widthPercentOfDefault, height - 130.0f*heightPercentOfDefault, 0.0f),
		"Save Changes",
		fontSize,
		TextColor,
		"",
		[this, a, i]() {
		a->PlaySoundEffect(L"menuclick.wav", 1);
		//ChangeStateTo(StateChange(new MenuState));
		this->_changes = 0;
	});


	_controller->ToggleVisible(b1, false);
	_controller->ToggleEventChecking(b1, false);

	// Back button
	Entity b2 = _builder->CreateButton(
		XMFLOAT3(50.0f*widthPercentOfDefault, height - 80.0f*heightPercentOfDefault, 0.0f),
		"Back",
		fontSize,
		TextColor,
		"",
		[]() {

	});

	// Discard changes text
	Entity sh = _builder->CreateLabel(
		XMFLOAT3(50.0f*widthPercentOfDefault, height - 80.0f*heightPercentOfDefault, 0.0f),
		"Discard changes?",
		fontSize,
		TextColor,
		250.0f,
		45.0f,
		"");

	Entity byes = _builder->CreateButton(
		XMFLOAT3(50.0f*widthPercentOfDefault + _builder->Text()->GetLength(sh)+20.0f*heightPercentOfDefault, height - 80.0f*heightPercentOfDefault, 0.0f),
		"Yes",
		fontSize,
		TextColor,
		"",
		[a]() {
		a->PlaySoundEffect(L"menuclick.wav", 1);
		ChangeStateTo(StateChange(new MenuState));
	});

	Entity bno = _builder->CreateButton(
		XMFLOAT3(50.0f*widthPercentOfDefault + _builder->Text()->GetLength(sh) + 40.0f*heightPercentOfDefault + _builder->Text()->GetLength(byes), height - 80.0f*heightPercentOfDefault, 0.0f),
		"No",
		fontSize,
		TextColor,
		"",
		[]() {

	});

	_controller->BindEvent(bno, EventManager::EventType::LeftClick,
		[b2, sh,bno,byes,a,c]() {
		a->PlaySoundEffect(L"menuclick.wav", 1);
		c->ToggleVisible(sh, false);
		c->ToggleEventChecking(sh, false);

		c->ToggleVisible(byes, false);
		c->ToggleEventChecking(byes, false);

		c->ToggleVisible(bno, false);
		c->ToggleEventChecking(bno, false);

		c->ToggleVisible(b2, true);
		c->ToggleEventChecking(b2, true);

	});

	_controller->ToggleVisible(sh, false);
	_controller->ToggleEventChecking(sh, false);

	_controller->ToggleVisible(byes, false);
	_controller->ToggleEventChecking(byes, false);

	_controller->ToggleVisible(bno, false);
	_controller->ToggleEventChecking(bno, false);

	

	_controller->BindEvent(b2, EventManager::EventType::LeftClick,
		[a, this, i, b2,byes,bno,sh]() {
		a->PlaySoundEffect(L"menuclick.wav", 1);
		if (this->_HasChanges())
		{

			_controller->ToggleVisible(b2, false);
			_controller->ToggleEventChecking(b2, false);

			_controller->ToggleVisible(sh, true);
			_controller->ToggleEventChecking(sh, true);


			_controller->ToggleVisible(byes, true);
			_controller->ToggleEventChecking(byes, true);

			_controller->ToggleVisible(bno, true);
			_controller->ToggleEventChecking(bno, true);
		}
		else
		{
		
			ChangeStateTo(StateChange(new MenuState));
		}
	});

	//==================
	//====	Grafics	====
	//==================
	OffsetY += 50 * heightPercentOfDefault;
	Entity graficsOptions = _builder->CreateLabel(
		XMFLOAT3(OffsetX - 550.0f*widthPercentOfDefault, OffsetY, 0.0f),
		"Grafic Options:",
		fontSize,
		TextColor,
		350.0f*widthPercentOfDefault,
		50.0f*heightPercentOfDefault,
		"");

	// Fullscreen
	std::vector<std::string> v;
	v.push_back("True");
	v.push_back("False");
	uint val = (o->GetFullscreen()) ? 0 : 1;

	OffsetY += 50 * heightPercentOfDefault;
	Entity fullscreen = _builder->CreateListSelection(
		XMFLOAT3(OffsetX - 350.0f*widthPercentOfDefault, OffsetY, 0.0f),
		std::string("Fullscreen:"),
		fontSize,
		v,
		val,
		350.0f*widthPercentOfDefault,
		400.0f*widthPercentOfDefault,
		[this, b1]()
	{
		this->_changes++;
		this->_controller->ToggleVisible(b1, true);
		this->_controller->ToggleEventChecking(b1, true);
	}, TextColor);



	// Resolution

	std::vector<std::string> v2;

	v2.push_back("800x600");
	v2.push_back("1280x720");
	v2.push_back("1280x1024");
	v2.push_back("1600x900");
	v2.push_back("1600x1024");
	v2.push_back("1680x1050");
	v2.push_back("1920x1080");

	if ((uint)width == 800)
		val = 0;
	else if ((uint)width == 1280)
	{
		if ((uint)height == 720)
			val = 1;
		else
			val = 2;
	}
	else if ((uint)width == 1600)
	{
		if ((uint)height == 900)
			val = 3;
		else
			val = 4;
	}
	else if ((uint)width == 1680)
		val = 5;
	else if ((uint)width == 1920)
		val = 6;

		//Change OffsetY for Resolution
	OffsetY += 50 * heightPercentOfDefault;

	Entity resolution = _builder->CreateListSelection(
		XMFLOAT3(width / 2.0f - 350.0f*widthPercentOfDefault, OffsetY, 0.0f),
		std::string("Resolution:"),
		fontSize,
		v2,
		val,
		350.0f*widthPercentOfDefault,
		400.0f*widthPercentOfDefault,
		[this, b1]()
	{
		this->_changes++;
		this->_controller->ToggleVisible(b1, true);
		this->_controller->ToggleEventChecking(b1, true);
	}, TextColor);

	val = (o->GetVsync()) ? 0 : 1;

	// vsync
	OffsetY += 50 * heightPercentOfDefault;
	Entity vsync = _builder->CreateListSelection(
		XMFLOAT3(width / 2.0f - 350.0f*widthPercentOfDefault, OffsetY, 0.0f),
		std::string("Vsync:"),
		fontSize,
		v,
		val,
		350.0f*widthPercentOfDefault,
		400.0f*widthPercentOfDefault,
		[this, b1]()
	{
		this->_changes++;
		this->_controller->ToggleVisible(b1, true);
		this->_controller->ToggleEventChecking(b1, true);
	}, TextColor);

	// FoV
	OffsetY += 50 * heightPercentOfDefault;
	Entity fov = _builder->CreateSlider(
		XMFLOAT3(width / 2.0f - 350.0f*widthPercentOfDefault, OffsetY, 0.0f),
		345.0f*widthPercentOfDefault,
		50.0f*heightPercentOfDefault,
		50.0f,
		150.0f,
		(float)o->GetFoV(),
		300.0f*widthPercentOfDefault,
		false,
		"FoV:",
		fontSize,
		355.0f*widthPercentOfDefault,
		[this,b1]() 
	{
		this->_changes++;
		this->_controller->ToggleVisible(b1, true);
		this->_controller->ToggleEventChecking(b1, true);
	}, TextColor);

	// Gamma
	OffsetY += 50 * heightPercentOfDefault;
	Entity gamma = _builder->CreateSlider(
		XMFLOAT3(width / 2.0f - 350.0f*widthPercentOfDefault, OffsetY, 0.0f),
		345.0f*widthPercentOfDefault,
		50.0f*heightPercentOfDefault,
		1.0f,
		4.0f,
		(float)o->GetGamma(),
		300.0f*widthPercentOfDefault,
		true,
		"Gamma:",
		fontSize,
		355.0f*widthPercentOfDefault,
		[this, b1]()
	{
		this->_changes++;
		this->_controller->ToggleVisible(b1, true);
		this->_controller->ToggleEventChecking(b1, true);
	}, TextColor);






	//======================
	//====	Game Play	====
	//======================

	OffsetY += 2*50 * heightPercentOfDefault;
	Entity gamePlayOptions = _builder->CreateLabel(
		XMFLOAT3(OffsetX - 550.0f*widthPercentOfDefault, OffsetY, 0.0f),
		"Game Play Options:",
		fontSize,
		TextColor,
		350.0f*widthPercentOfDefault,
		50.0f*heightPercentOfDefault,
		"");

	// Weapon mode
	val = o->GetWeaponMode();

	std::vector<std::string> v3;

	v3.push_back("Circular");
	v3.push_back("List");

	OffsetY += 50 * heightPercentOfDefault;
	Entity wepmode = _builder->CreateListSelection(
		XMFLOAT3(width / 2.0f - 350.0f*widthPercentOfDefault, OffsetY, 0.0f),
		std::string("Weapon Mode:"),
		fontSize,
		v3,
		val,
		350.0f*widthPercentOfDefault,
		400.0f*widthPercentOfDefault,
		[this, b1]()
	{
		this->_changes++;
		this->_controller->ToggleVisible(b1, true);
		this->_controller->ToggleEventChecking(b1, true);
	}, TextColor);


	//Difficulty Mode
	val = o->GetDifficulty();

	std::vector<std::string> v4;
	v4.push_back("Easy");
	v4.push_back("Normal");
	v4.push_back("Hard");
	v4.push_back("Very Hard");
	OffsetY += 50 * heightPercentOfDefault;

	Entity difficultyMode = _builder->CreateListSelection(
		XMFLOAT3(width / 2.0f - 350.0f*widthPercentOfDefault, OffsetY, 0.0f),
		std::string("Difficulty:"),
		fontSize,
		v4,
		val,
		350.0f*widthPercentOfDefault,
		405.0f*widthPercentOfDefault,
		[this, b1]()
	{
		this->_changes++;
		this->_controller->ToggleVisible(b1, true);
		this->_controller->ToggleEventChecking(b1, true);
	}, TextColor);



	//Hardcore Mode
	OffsetY += 50 * heightPercentOfDefault;
	val = (o->GetHardcoreMode()) ? 0 : 1;

	Entity hardcoreMode = _builder->CreateListSelection(
		XMFLOAT3(width / 2.0f - 350.0f, OffsetY, 0.0f),
		std::string("Hardcore Mode:"),
		fontSize,
		v,
		val,
		350.0f*widthPercentOfDefault,
		400.0f*widthPercentOfDefault,
		[this, b1]()
	{
		this->_changes++;
		this->_controller->ToggleVisible(b1, true);
		this->_controller->ToggleEventChecking(b1, true);
	}, TextColor);


	//==========================
	//====	Audio Options	====
	//==========================

	OffsetY += 2*50 * heightPercentOfDefault;
	Entity Audio = _builder->CreateLabel(
		XMFLOAT3(width / 2.0f - 550.0f*widthPercentOfDefault, OffsetY, 0.0f),
		"Audio Options:",
		fontSize,
		TextColor,
		350.f*widthPercentOfDefault,
		45.0f*heightPercentOfDefault,
		"");


	// Master Audio
	OffsetY += 50 * heightPercentOfDefault;
	Entity ma = _builder->CreateSlider(
		XMFLOAT3(width / 2.0f - 350.0f*widthPercentOfDefault, OffsetY, 0.0f),
		345.0f*widthPercentOfDefault,
		50.0f*widthPercentOfDefault,
		0.0f,
		100.0f,
		(float)o->GetMasterVolume()*100,
		300.0f*widthPercentOfDefault,
		false,
		"Master:",
		fontSize,
		355.0f*widthPercentOfDefault,
		[this, b1]()
	{
		this->_changes++;
		this->_controller->ToggleVisible(b1, true);
		this->_controller->ToggleEventChecking(b1, true);
	}, TextColor);


	// Music Audio
	OffsetY += 50 * heightPercentOfDefault;
	Entity mua = _builder->CreateSlider(
		XMFLOAT3(width / 2.0f - 350.0f*widthPercentOfDefault, OffsetY, 0.0f),
		345.0f*widthPercentOfDefault,
		50.0f*widthPercentOfDefault,
		0.0f,
		100.0f,
		(float)o->GetMusicVolume() * 100,
		300.0f*widthPercentOfDefault,
		false,
		"Music:",
		fontSize,
		355.0f*widthPercentOfDefault,
		[this, b1]()
	{
		this->_changes++;
		this->_controller->ToggleVisible(b1, true);
		this->_controller->ToggleEventChecking(b1, true);
	}, TextColor);


	// Effect Audio
	OffsetY += 50 * heightPercentOfDefault;
	Entity ea = _builder->CreateSlider(
		XMFLOAT3(width / 2.0f - 350.0f*widthPercentOfDefault, OffsetY, 0.0f),
		350.0f*widthPercentOfDefault,
		50.0f*widthPercentOfDefault,
		0.0f,
		100.0f,
		(float)o->GetSoundEffectVolume() * 100,
		300.0f*widthPercentOfDefault,
		false,
		"Effects:",
		fontSize,
		355.0f*widthPercentOfDefault,
		[this, b1]()
	{
		this->_changes++;
		this->_controller->ToggleVisible(b1, true);
		this->_controller->ToggleEventChecking(b1, true);
	}, TextColor);


	_controller->BindEvent(b1,
		EventManager::EventType::LeftClick,
		[this, a, b1, o, fullscreen, resolution, vsync,fov, ma, mua,ea, wepmode, difficultyMode, hardcoreMode,gamma]()
	{
		//Get fullsceen info
		a->PlaySoundEffect(L"menuclick.wav", 1);
		bool temp = (this->_controller->GetListSelectionValue(fullscreen) == 0) ? true : false;
		o->SetFullscreen(temp);


		// Get resolution info
		switch (this->_controller->GetListSelectionValue(resolution))
		{
		case 0:
			o->SetScreenResolutionWidth(800);
			o->SetScreenResolutionHeight(600);
			o->SetAspectRatio(800.0f / 600.0f);
			break;
		case 1:
			o->SetScreenResolutionWidth(1280);
			o->SetScreenResolutionHeight(720);
			o->SetAspectRatio(1280.0f / 720);
			break;
		case 2:
			o->SetScreenResolutionWidth(1280);
			o->SetScreenResolutionHeight(1024);
			o->SetAspectRatio(1280.0f / 1024.0f);
			break;
		case 3:
			o->SetScreenResolutionWidth(1600);
			o->SetScreenResolutionHeight(900);
			o->SetAspectRatio(1600.0f / 900.0f);
			break;
		case 4:
			o->SetScreenResolutionWidth(1600);
			o->SetScreenResolutionHeight(1024);
			o->SetAspectRatio(1600.0f / 1024.0f);
			break;
		case 5:
			o->SetScreenResolutionWidth(1680);
			o->SetScreenResolutionHeight(1050);
			o->SetAspectRatio(1680.0f / 1050.0f);
			break;
		case 6:
			o->SetScreenResolutionWidth(1920);
			o->SetScreenResolutionHeight(1080);
			o->SetAspectRatio(1920.0f / 1080.0f);
			break;
		default:
			break;
		}

		temp = (this->_controller->GetListSelectionValue(vsync) == 0) ? true : false;
		o->SetVsync(temp);

		// FoV
		float ftemp = this->_controller->GetSliderValue(fov);
		o->SetFoV((uint)ftemp);

		ftemp = this->_controller->GetSliderValue(gamma);
		o->SetGamma(ftemp);

		// Weapon Mode
		uint itemp = this->_controller->GetListSelectionValue(wepmode);
		o->SetWeaponMode(itemp);

		//Difficulty Mode
		uint difficulty = this->_controller->GetListSelectionValue(difficultyMode);
		o->SetDifficulty(difficulty);

		//Hardcore Mode
		temp = (this->_controller->GetListSelectionValue(hardcoreMode) == 0) ? true : false;
		o->SetHardcoreMode(temp);

		// Master audio
		ftemp = this->_controller->GetSliderValue(ma);
		o->SetMasterVolume(ftemp / 100.0f);


		// Music audio
		ftemp = this->_controller->GetSliderValue(mua);
		o->SetMusicVolume(ftemp / 100.0f);

		// Effect audio
		ftemp = this->_controller->GetSliderValue(ea);
		o->SetSoundEffectVolume(ftemp / 100.0f);

		System::GetInstance()->ToggleFullscreen();
		ChangeStateTo(StateChange(new OptionsState));

	});

	//std::vector<Item> items;
	//for (uint k =  0; k < 10; k++)
	//{
	//	Entity e = _builder->CreateListSelection(
	//		XMFLOAT3(0.0f,0.0f,0.0f),
	//		std::string("Test: " + to_string(k)),
	//		v,
	//		val,
	//		250.0f,
	//		300.0f,
	//		[this]()
	//	{
	//		this->_changes++;
	//	}, TextColor);

	//	items.push_back(std::move(Item(e, ItemType::ListSelection)));
	//}



	//Entity sl = _builder->CreateScrollList(
	//	XMFLOAT3(width / 2.0f - 550.0f, 500.0f, 0.0f),
	//	550.0f,
	//	300.0f,
	//	50.0f,
	//	items);


}

void OptionsState::Shutdown()
{
}
