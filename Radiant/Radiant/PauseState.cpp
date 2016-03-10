#include "PauseState.h"
#include "System.h"


PauseState::PauseState()
{
}


PauseState::~PauseState()
{

}

void PauseState::Init()
{
	auto o = System::GetOptions();
	float width = (float)o->GetScreenResolutionWidth();
	float height = (float)o->GetScreenResolutionHeight();
	auto i = System::GetInput();
	auto c = _controller;
	auto a = System::GetInstance()->GetAudio();
	XMFLOAT4 TextColor = XMFLOAT4(0.56f, 0.26f, 0.15f, 1.0f);

	// Radiant text
	Entity text  = _builder->CreateLabel(
		XMFLOAT3(width / 2.0f - 110.0f, 25.0f, 0.0f),
		"Game Paused",
		TextColor,
		250.0f,
		45.0f,
		"");
	_builder->Transform()->SetPosition(text, XMFLOAT3(width / 2.0f - _builder->Text()->GetLength(text) / 2.0f, 25.0f, 0.0f));

	// Resume button
	Entity b1 = _builder->CreateButton(
		XMFLOAT3(width / 2.0f - 110.0f, height/4.0f, 0.0f),
		"Resume Game",
		TextColor,
		250.0f,
		45.0f,
		"",
		[]()
	{

	});

	_builder->Transform()->SetPosition(b1, XMFLOAT3(width / 2.0f - _builder->Text()->GetLength(b1) / 2.0f, height / 1.5f, 0.0f));

	//Main menu button
	Entity b5 = _builder->CreateButton(
		XMFLOAT3(width / 2.0f - 110.0f, height / 4.0f, 0.0f),
		"Main Menu",
		TextColor,
		250.0f,
		45.0f,
		"",
		[i, a]()
	{
		a->PlaySoundEffect(L"menuclick.wav", 1);
		ChangeStateTo(StateChange(new MenuState()));
	});

	_builder->Transform()->SetPosition(b5, XMFLOAT3(width / 2.0f - _builder->Text()->GetLength(b5) / 2.0f, height / 1.5f + 55.0f, 0.0f));

	// Exit button
	Entity b2 = _builder->CreateButton(
		XMFLOAT3(width / 2.0f - 110.0f, height / 4.0f + 55.0f, 0.0f),
		"Exit to desktop",
		TextColor,
		250.0f,
		45.0f,
		"",
		[a]() {
		a->PlaySoundEffect(L"menuclick.wav", 1);
		ExitApplication;
	});
	_builder->Transform()->SetPosition(b2, XMFLOAT3(width / 2.0f - _builder->Text()->GetLength(b2)/2.0f, height / 1.5f + 110.0f, 0.0f));

	_controller->BindEvent(b1, EventManager::EventType::LeftClick,
		[i, a, this, text, b1, b2, b5]()
	{
		_controller->ReleaseEntity(text);
		_controller->ReleaseEntity(b1);
		_controller->ReleaseEntity(b2);
		_controller->ReleaseEntity(b5);
		i->LockMouseToCenter(true);
		i->LockMouseToWindow(true);
		i->HideCursor(true);
		i->MouseUp(VK_LBUTTON);
		a->PlaySoundEffect(L"menuclick.wav", 1);
		ChangeStateTo(StateChange(_savedState,false,true,true));
		_savedState = nullptr;
	});
	_controller->BindEvent(b1, EventManager::EventType::Update, [i, a, this, text, b1, b2, b5]()
	{
		if (i->IsKeyPushed(VK_ESCAPE))
		{
			_controller->ReleaseEntity(text);
			_controller->ReleaseEntity(b1);
			_controller->ReleaseEntity(b2);
			_controller->ReleaseEntity(b5);
			i->LockMouseToCenter(true);
			i->LockMouseToWindow(true);
			i->HideCursor(true);
			i->MouseUp(VK_LBUTTON);
			a->PlaySoundEffect(L"menuclick.wav", 1);
			ChangeStateTo(StateChange(_savedState, false, true, true));
			_savedState = nullptr;
		}
	});

}

void PauseState::Shutdown()
{
	SAFE_SHUTDOWN(_savedState);
}