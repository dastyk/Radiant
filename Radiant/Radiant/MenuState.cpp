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
	auto i = System::GetInput();

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

	Entity b1 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 200.0f, 0.0f),
		"Start Game",
		XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f),
		250.0f,
		45.0f,
		"",
		[i](){
		i->LockMouseToCenter(true);
		i->LockMouseToWindow(true);
		i->HideCursor(true);
		throw StateChange(new GameState());
	});

	auto c = _controller;
	_controller->BindOnEnterEvent(b1,
		[b1, c](){
		c->Text()->ChangeColor(b1,XMFLOAT4(0.3f, 0.5f, 0.8f, 1.0f));
	});

	_controller->BindOnExitEvent(b1,
		[b1, c]() {
		c->Text()->ChangeColor(b1, XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f));
	});

	Entity b2 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 100.0f, 0.0f),
		"Exit",
		XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f),
		250.0f,
		45.0f,
		"",
		[]() {
		throw FinishMsg(1);
	});

	_controller->BindOnEnterEvent(b2,
		[b2, c]() {
		c->Text()->ChangeColor(b2, XMFLOAT4(0.3f, 0.5f, 0.8f, 1.0f));
	});

	_controller->BindOnExitEvent(b2,
		[b2, c]() {
		c->Text()->ChangeColor(b2, XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f));
	});


	Entity b3 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 150.0f, 0.0f),
		"Test State",
		XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f),
		250.0f,
		50.0f,
		"",
		[i]() {
		i->LockMouseToCenter(true);
		i->LockMouseToWindow(true);
		i->HideCursor(true);
		throw StateChange(new TestState());
	});

	_controller->BindOnEnterEvent(b3,
		[b3, c]() {
		c->Text()->ChangeColor(b3, XMFLOAT4(0.3f, 0.5f, 0.8f, 1.0f));
	});

	_controller->BindOnExitEvent(b3,
		[b3, c]() {
		c->Text()->ChangeColor(b3, XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f));
	});
}

void MenuState::Shutdown()
{
}

