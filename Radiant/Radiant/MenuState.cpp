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
	_StartGameButton = _builder->CreateButton(
		XMFLOAT3(300.0f, 200.0f,0.0f),
		"Start Game",
		XMFLOAT4(1.0f,0.0f,0.0f,1.0f),
		150.0f,
		50.0f,
		"",
		[this](){
		throw FinishMsg(1);		
	});
}

void MenuState::Shutdown()
{
}

void MenuState::HandleInput()
{
	_controller->HandleInput();
}

void MenuState::Update()
{
}

void MenuState::Render()
{
	System::GetGraphics()->Render(_gameTimer.TotalTime(), _gameTimer.DeltaTime());
}
