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
	auto h = System::GetWindowHandler();
	float width = (float)h->GetWindowWidth();
	float height = (float)h->GetWindowHeight();


	_builder->CreateImage(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		width,
		height,
		"Assets/Textures/conceptArt.png");

	_builder->CreateLabel(
		XMFLOAT3(width / 2.0f - 100.0f, 25.0f, 0.0f),
		"Radiant",
		XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f),
		250.0f,
		45.0f,
		"");

	_builder->CreateButton(
		XMFLOAT3(50.0f, height - 200.0f, 0.0f),
		"Start Game",
		XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f),
		250.0f,
		45.0f,
		"",
		[this](){
		System::GetInput()->LockMouseToCenter(true);
		System::GetInput()->LockMouseToWindow(true);
		System::GetInput()->HideCursor(true);
		throw StateChange(new GameState());
	});

	_builder->CreateButton(
		XMFLOAT3(50.0f, height - 100.0f, 0.0f),
		"Exit",
		XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f),
		250.0f,
		45.0f,
		"",
		[this]() {
		throw FinishMsg(1);
	});

	_builder->CreateButton(
		XMFLOAT3(50.0f, height - 150.0f, 0.0f),
		"Test State",
		XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f),
		250.0f,
		50.0f,
		"",
		[this]() {
		System::GetInput()->LockMouseToCenter(true);
		System::GetInput()->LockMouseToWindow(true);
		System::GetInput()->HideCursor(true);
		throw StateChange(new TestState());
	});
}

void MenuState::Shutdown()
{
}

