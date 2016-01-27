#include "MenuState.h"
#include "System.h"
#include "Graphics.h"

using namespace DirectX;

MenuState::MenuState() : State()
{

}


MenuState::~MenuState()
{
	delete _staticMeshManager;
	delete _transformManager;
}


void MenuState::Init()
{
	_transformManager = new TransformManager();
	_staticMeshManager = new StaticMeshManager( *System::GetGraphics(), *_transformManager );

	_BTH = _entityManager.Create();
	_staticMeshManager->CreateStaticMesh( _BTH, "Assets/Models/bth.arf" );
	_transformManager->CreateTransform( _BTH );
	_transformManager->SetTransform( _BTH, XMMatrixScaling( 1.0f, 1.0f, 1.0f ) );
	//System::GetCollision()->CreateBBT(_BTH);

	_anotherOne = _entityManager.Create();
	_staticMeshManager->CreateStaticMesh( _anotherOne, "Assets/Models/test.arf" );
	_transformManager->CreateTransform( _anotherOne );
	_transformManager->SetTransform( _anotherOne, XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation( 4.0f, 0.0f, 0.0f ) );
	_transformManager->BindChild( _BTH, _anotherOne );
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

	XMMATRIX transform = _transformManager->GetTransform( _BTH );
	transform *= XMMatrixRotationY( 0.0167f * 0.1f * XM_PIDIV4 );
	_transformManager->SetTransform( _BTH, transform );

	transform = _transformManager->GetTransform( _anotherOne );
	transform = XMMatrixRotationY( 0.0167f * 0.4f * XM_PIDIV4 ) * transform;
	_transformManager->SetTransform( _anotherOne, transform );
}

void MenuState::Render()
{
	System::GetGraphics()->Render( _gameTimer.TotalTime(), _gameTimer.DeltaTime() );
}