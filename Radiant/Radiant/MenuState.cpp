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
	if(System::GetInstance()->_inputInst->IsKeyDown(VK_W))
		throw FinishMsg(1, L"END");
}

void MenuState::Update()
{
}

void MenuState::Render()
{


}