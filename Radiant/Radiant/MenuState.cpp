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
	delete _cameraManager;
}


void MenuState::Init()
{
	_transformManager = new TransformManager();
	_staticMeshManager = new StaticMeshManager( *System::GetGraphics(), *_transformManager );
	_cameraManager = new CameraManager(*System::GetGraphics(), *_transformManager);

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

	_camera = _entityManager.Create();

	_cameraManager->CreateCamera(_camera);
	_transformManager->CreateTransform(_camera);
	_transformManager->SetLookDir(_camera, XMVectorSet(0, 0, 1, 0));
	_cameraManager->SetActivePerspective(_camera);
	//_cameraView = XMMatrixLookAtLH(XMVectorSet(0, 0, -50, 1), XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0));
	//_cameraProj = XMMatrixPerspectiveFovLH(0.25f * XM_PI, 800.0f / 600.0f, 0.1f, 1000.0f);

	System::GetInput()->ToggleLockMouseToCenter();
	System::GetInput()->ToggleLockMouseToWindow();
	System::GetInput()->HideCursor(true);
}

void MenuState::Shutdown()
{
	State::Shutdown();
	System::GetInput()->ToggleLockMouseToCenter();
	System::GetInput()->ToggleLockMouseToWindow();
	System::GetInput()->HideCursor(false);
}

void MenuState::HandleInput()
{
	if(System::GetInput()->IsKeyDown(VK_ESCAPE))
		throw FinishMsg(1);
	if (System::GetInput()->GetKeyStateAndReset(VK_SPACE))
		System::GetInput()->ToggleLockMouseToWindow();
	if (System::GetInput()->IsKeyDown(VK_W))
		_transformManager->MoveForward(_camera, 10*_gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_S))
		_transformManager->MoveBackward(_camera,10 *_gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_A))
		_transformManager->MoveLeft(_camera, 10 * _gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_D))
		_transformManager->MoveRight(_camera, 10 * _gameTimer.DeltaTime());

	int x, y;
	System::GetInput()->GetMouseDiff(x, y);
	_transformManager->RotateYaw(_camera, x*_gameTimer.DeltaTime());
	_transformManager->RotatePitch(_camera, y*_gameTimer.DeltaTime());
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