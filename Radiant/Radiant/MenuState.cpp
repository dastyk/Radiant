#include "MenuState.h"



MenuState::MenuState() : State()
{
}


MenuState::~MenuState()
{
}

void MenuState::Init()
{
	_StartGameButton = _builder->CreateButton(
		XMVectorSet(300.0f, 200.0f,0.0f,0.0f),
		"Start Game",
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
}

void MenuState::Update()
{
}

void MenuState::Render()
{
}
