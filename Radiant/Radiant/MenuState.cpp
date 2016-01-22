#include "MenuState.h"
#include "System.h"


MenuState::MenuState()
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
}

void MenuState::HandleInput()
{
	if(System::GetInstance()->GetInput()->IsKeyDown(VK_ESCAPE))
		throw FinishMsg(1);
}

void MenuState::Update()
{
}

void MenuState::Render()
{


}