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

	auto i = System::GetInput();
	auto o = System::GetOptions();
	float width = (float)o->GetScreenResolutionWidth();
	float height = (float)o->GetScreenResolutionHeight();
	auto c = _controller;
	auto a = System::GetInstance()->GetAudio();

	// Background image
	_builder->CreateImage(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		width,
		height,
		"Assets/Textures/conceptArt.png");


	// Options text
	_builder->CreateLabel(
		XMFLOAT3(width / 2.0f - 100.0f, 25.0f, 0.0f),
		"Options",
		XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f),
		250.0f,
		45.0f,
		"");

	// Save Changes button
	Entity b1 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 150.0f, 0.0f),
		"Save Changes",
		XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f),
		250.0f,
		45.0f,
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
		XMFLOAT3(50.0f, height - 100.0f, 0.0f),
		"Back",
		XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f),
		250.0f,
		45.0f,
		"",
		[a, this, i, b1]() {
		if (this->_HasChanges())
		{
			this->_controller->Text()->ChangeText(b1, "Save your changes you pleb.");
		}
		else
		{
			a->PlaySoundEffect(L"menuclick.wav", 1);
			ChangeStateTo(StateChange(new MenuState));
		}
	});

	std::vector<std::string> v;
	v.push_back("True");
	v.push_back("False");
	uint val = (o->GetFullscreen()) ? 0 : 1;
	// Fullscreen
	Entity fullscreen = _builder->CreateListSelection(
		XMFLOAT3(width / 2.0f - 350.0f, 75.0f, 0.0f),
		std::string("Fullscreen:"),
		v,
		val,
		250.0f,
		300.0f,
		[this, b1]()
	{
		this->_changes++;
		this->_controller->ToggleVisible(b1, true);
		this->_controller->ToggleEventChecking(b1, true);
	});

	v.clear();

	v.push_back("800x600");
	v.push_back("1280x720");
	v.push_back("1280x1024");
	v.push_back("1600x900");
	v.push_back("1600x1024");
	v.push_back("1680x1050");
	v.push_back("1920x1080");

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

	// Resolution
	Entity resolution = _builder->CreateListSelection(
		XMFLOAT3(width / 2.0f - 350.0f, 125.0f, 0.0f),
		std::string("Resolution:"),
		v,
		val,
		250.0f,
		300.0f,
		[this, b1]()
	{
		this->_changes++;
		this->_controller->ToggleVisible(b1, true);
		this->_controller->ToggleEventChecking(b1, true);
	});




	_controller->BindEvent(b1,
		EventManager::EventType::LeftClick,
		[this, a, b1, o, fullscreen, resolution]()
	{
		//Get fullsceen info
		a->PlaySoundEffect(L"menuclick.wav", 1);
		bool full = (this->_controller->GetListSelectionValue(fullscreen) == 0) ? true : false;
		o->SetFullscreen(full);


		// Get resolution info
		switch (this->_controller->GetListSelectionValue(resolution))
		{
		case 0:
			o->SetScreenResolutionWidth(800);
			o->SetScreenResolutionHeight(600);
			break;
		case 1:
			o->SetScreenResolutionWidth(1280);
			o->SetScreenResolutionHeight(720);
			break;
		case 2:
			o->SetScreenResolutionWidth(1280);
			o->SetScreenResolutionHeight(1024);
			break;
		case 3:
			o->SetScreenResolutionWidth(1600);
			o->SetScreenResolutionHeight(900);
			break;
		case 4:
			o->SetScreenResolutionWidth(1600);
			o->SetScreenResolutionHeight(1024);
			break;
		case 5:
			o->SetScreenResolutionWidth(1680);
			o->SetScreenResolutionHeight(1050);
			break;
		case 6:
			o->SetScreenResolutionWidth(1920);
			o->SetScreenResolutionHeight(1080);
			break;
		default:
			break;
		}




		System::GetInstance()->ToggleFullscreen();
		ChangeStateTo(StateChange(new OptionsState));

	});
}

void OptionsState::Shutdown()
{
}
