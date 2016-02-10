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
		[a,i]() {
		a->PlaySoundEffect(L"menuclick.wav", 1);
		ChangeStateTo(StateChange(new MenuState));
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
	_controller->ToggleVisible(b1, false);
	_controller->ToggleEventChecking(b1, false);
	// Exit button
	Entity b2 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 100.0f, 0.0f),
		"Exit",
		XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f),
		250.0f,
		45.0f,
		"",
		[a,this,i]() {
		if (this->_HasChanges())
		{
			// Promt if you really want to discard changes
		}
		a->PlaySoundEffect(L"menuclick.wav", 1);
		ChangeStateTo(StateChange(new MenuState));
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



}

void OptionsState::Shutdown()
{
}
