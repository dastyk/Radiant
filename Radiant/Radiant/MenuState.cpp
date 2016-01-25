#include "MenuState.h"
#include "System.h"
#include "Graphics.h"

MenuState::MenuState() : State()
{

}


MenuState::~MenuState()
{
}


void MenuState::Init()
{
}

void MenuState::ShutDown()
{
	State::ShutDown();
}

void MenuState::HandleInput()
{
	if(System::GetInstance()->GetInput()->IsKeyDown(VK_ESCAPE))
		throw FinishMsg(1);
	if (System::GetInstance()->GetInput()->GetKeyStateAndReset(VK_SPACE))
		System::GetInstance()->GetInput()->ToggleLockMouseToCenter();
}

void MenuState::Update()
{
}

void MenuState::Render()
{
	System::GetInstance()->GetGraphics()->Render( 0.0, 0.0 );
}