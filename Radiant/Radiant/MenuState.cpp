#include "MenuState.h"
#include "System.h"


MenuState::MenuState() : State()
{
}


MenuState::~MenuState()
{
}

void MenuState::Init()
{
	auto o = System::GetOptions();
	float width = (float)o->GetScreenResolutionWidth();
	float height = (float)o->GetScreenResolutionHeight();
	auto i = System::GetInput();
	auto c = _controller;
	auto a = System::GetInstance()->GetAudio();

	XMFLOAT4 TextColor = XMFLOAT4(0.56f, 0.26f, 0.15f, 1.0f);



	// Background image
	_builder->CreateImage(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		width,
		height,
		"Assets/Textures/conceptArt.png");

	// Radiant text
	_builder->CreateLabel(
		XMFLOAT3(width / 2.0f - 100.0f, 25.0f, 0.0f),
		"Radiant",
		TextColor,
		250.0f,
		45.0f,
		"");

	// Start game button
	Entity b1 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 230.0f, 0.0f),
		"Start Game",
		TextColor,
		250.0f,
		45.0f,
		"",
		[i, a]()
	{
		a->PlaySoundEffect(L"menuclick.wav", 1);
		i->LockMouseToCenter(true);
		i->LockMouseToWindow(true);
		i->HideCursor(true);
		ChangeStateTo(StateChange(new GameState()));
	});


	//Options button
	Entity b5 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 180.0f, 0.0f),
		"Options",
		TextColor,
		250.0f,
		45.0f,
		"",
		[i, a]()
	{
		a->PlaySoundEffect(L"menuclick.wav", 1);
		ChangeStateTo(StateChange(new OptionsState));
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

	_controller->BindEvent(b2, EventManager::EventType::Update, [i]()
	{
		if (i->IsKeyPushed(VK_ESCAPE))
		{
			ExitApplication;
		}
		if (i->IsKeyPushed('2'))
		{
			i->LockMouseToCenter(true);
			i->LockMouseToWindow(true);
			i->HideCursor(true);
			ChangeStateTo(StateChange(new TestState()));
		}
	}
	);



	// Test State button
	Entity b3 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 130.0f, 0.0f),
		"Test State",
		TextColor,
		250.0f,
		50.0f,
		"",
		[i,a]() 
	{
		a->PlaySoundEffect(L"menuclick.wav", 1);
		i->LockMouseToCenter(true);
		i->LockMouseToWindow(true);
		i->HideCursor(true);
		ChangeStateTo(StateChange(new TestState()));
	});
}

void MenuState::Shutdown()
{
}

