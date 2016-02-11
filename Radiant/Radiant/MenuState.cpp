#include "MenuState.h"
#include "System.h"
#define TextColor XMFLOAT4(0.56f, 0.26f, 0.15f, 1.0f)


MenuState::MenuState() : State()
{
}


MenuState::~MenuState()
{
}

void MenuState::Init()
{
	auto h = System::GetWindowHandler();
	float width = (float)h->GetWindowWidth();
	float height = (float)h->GetWindowHeight();
	auto i = System::GetInput();
	auto c = _controller;
	auto a = System::GetInstance()->GetAudio();

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
		XMFLOAT3(50.0f, height - 250.0f, 0.0f),
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
	_controller->BindEvent(b1,
		EventManager::EventType::OnEnter,
		[b1, c, a]()
	{
		c->Text()->ChangeColor(b1, XMFLOAT4(0.3f, 0.5f, 0.8f, 1.0f));
		a->PlaySoundEffect(L"menuhover.wav", 1);
	});
	_controller->BindEvent(b1,
		EventManager::EventType::OnExit,
		[b1, c]()
	{
		c->Text()->ChangeColor(b1, XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f));
	});


	//Options button
	Entity b5 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 200.0f, 0.0f),
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
		XMFLOAT3(50.0f, height - 100.0f, 0.0f),
		"Exit",
		TextColor,
		250.0f,
		45.0f,
		"",
		[a]() {
		a->PlaySoundEffect(L"menuclick.wav", 1);
		throw FinishMsg(1);
	});
	_controller->BindEvent(b2,
		EventManager::EventType::OnEnter,
		[b2, c, a]()
	{
		c->Text()->ChangeColor(b2, XMFLOAT4(0.3f, 0.5f, 0.8f, 1.0f));
		a->PlaySoundEffect(L"menuhover.wav", 1);
	});
	_controller->BindEvent(b2,
		EventManager::EventType::OnExit,
		[b2, c]()
	{
		c->Text()->ChangeColor(b2, XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f));
	});
	_controller->BindEvent(b2, EventManager::EventType::Update, [i]()
	{
		if (i->IsKeyPushed(VK_ESCAPE))
		{
			throw FinishMsg(0);
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
		XMFLOAT3(50.0f, height - 150.0f, 0.0f),
		"Test State",
		XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f),
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

