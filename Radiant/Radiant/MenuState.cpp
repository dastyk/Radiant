#include "MenuState.h"
#include "System.h"
#include "Graphics.h"

MenuState::MenuState() : State()
{

}


MenuState::~MenuState()
{
	delete _staticMeshManager;
}


void MenuState::Init()
{
	_staticMeshManager = new StaticMeshManager( *System::GetGraphics() );

	_BTH = _entityManager.Create();
	_staticMeshManager->CreateStaticMesh( _BTH, "test.arf" );
	//System::GetCollision()->CreateBBT(_BTH);
	
}

void MenuState::Shutdown()
{
	State::Shutdown();
}

void MenuState::HandleInput()
{
	if(System::GetInput()->IsKeyDown(VK_ESCAPE))
		throw FinishMsg(1);
	if (System::GetInput()->GetKeyStateAndReset(VK_A))
		System::GetInput()->ToggleLockMouseToCenter();
	if (System::GetInput()->GetKeyStateAndReset(VK_SPACE))
		System::GetInput()->ToggleLockMouseToWindow();
	if (System::GetInput()->GetKeyStateAndReset(VK_W))
		System::GetInstance()->ToggleFullscreen();
}

void MenuState::Update()
{
	_gameTimer.Tick();
}

void MenuState::Render()
{
	System::GetGraphics()->Render( _gameTimer.TotalTime(), _gameTimer.DeltaTime() );
}