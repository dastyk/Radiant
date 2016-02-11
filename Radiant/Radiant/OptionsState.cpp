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
	auto h = System::GetWindowHandler();
	float width = (float)h->GetWindowWidth();
	float height = (float)h->GetWindowHeight();
	auto i = System::GetInput();
	auto o = System::GetOptions();
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
		[this,a,i]() {
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
		[a,this,i,b1]() {
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
	uint val = (o->GetFullscreen())?0:1;
	// Fullscreen
	Entity fullscreen = _builder->CreateListSelection(
		XMFLOAT3(200.0f, 200.0f, 0.0f),
		std::string("Fullscreen:"),
		v,
		val,
		250.0f,
		200.0f,
		[this, b1]() 
	{
		this->_changes++;
		this->_controller->ToggleVisible(b1, true);
		this->_controller->ToggleEventChecking(b1, true);
	});


	_controller->BindEvent(b1,
		EventManager::EventType::LeftClick,
		[this, a, b1, o, fullscreen,h]()
	{
		a->PlaySoundEffect(L"menuclick.wav", 1);
		bool full = (this->_controller->GetListSelectionValue(fullscreen) == 0) ? true : false;
		o->SetFullscreen(full);
		System::GetInstance()->ToggleFullscreen();
		ChangeStateTo(StateChange(new OptionsState));

	});
}

void OptionsState::Shutdown()
{
}
