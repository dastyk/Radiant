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

	_builder->CreateButton(
		XMFLOAT3(280.0f, 200.0f,0.0f),
		"Start Game",
		XMFLOAT4(1.0f,0.0f,0.0f,1.0f),
		150.0f,
		50.0f,
		"",
		[this](){
		System::GetInput()->LockMouseToCenter(true);
		System::GetInput()->LockMouseToWindow(true);
		System::GetInput()->HideCursor(true);
		throw StateChange(new GameState());
	});
	_builder->CreateButton(
		XMFLOAT3(340.0f, 250.0f, 0.0f),
		"Exit",
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		150.0f,
		50.0f,
		"",
		[this]() {
		throw FinishMsg(1);
	});

	_builder->CreateButton(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		"Test State",
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		150.0f,
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

