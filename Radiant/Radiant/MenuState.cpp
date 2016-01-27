#include "MenuState.h"
#include "System.h"
#include "Graphics.h"
#include "Audio.h"

MenuState::MenuState() : State()
{
	testAudio = new Audio;
	//test.PlayBGMusic(L"musictest.wav", 1);
	for (int i = 0; i < 4; i++)
	{
		testAudio->PlaySoundEffect(L"test.wav", 1);
		Sleep(200);
	}

}


MenuState::~MenuState()
{
	delete _staticMeshManager;
	delete testAudio;
}


void MenuState::Init()
{
	_staticMeshManager = new StaticMeshManager( *System::GetInstance()->GetGraphics() );

	_BTH = _entityManager.Create();
	//_staticMeshManager->CreateStaticMesh( _BTH, "Assets/Models/bth.obj" );
}

void MenuState::Shutdown()
{
	State::Shutdown();
}

void MenuState::HandleInput()
{
	if(System::GetInstance()->GetInput()->IsKeyDown(VK_ESCAPE))
		throw FinishMsg(1);
	if (System::GetInstance()->GetInput()->GetKeyStateAndReset(VK_A))
		System::GetInstance()->GetInput()->ToggleLockMouseToCenter();
	if (System::GetInstance()->GetInput()->GetKeyStateAndReset(VK_SPACE))
		System::GetInstance()->GetInput()->ToggleLockMouseToWindow();
	if (System::GetInstance()->GetInput()->GetKeyStateAndReset(VK_W))
		System::GetInstance()->ToggleFullscreen();
}

void MenuState::Update()
{
	_gameTimer.Tick();
}

void MenuState::Render()
{
	System::GetInstance()->GetGraphics()->Render( _gameTimer.TotalTime(), _gameTimer.DeltaTime() );
}