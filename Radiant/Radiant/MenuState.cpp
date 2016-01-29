#include "MenuState.h"
#include "System.h"
#include "Graphics.h"

using namespace DirectX;

MenuState::MenuState() : State()
{

}


MenuState::~MenuState()
{
}


void MenuState::Init()
{
	//System::GetInstance()->ToggleFullscreen();

	_BTH = _managers.entity.Create();
	_managers.mesh->CreateStaticMesh( _BTH, "Assets/Models/bth.arf" );
	_managers.transform->CreateTransform( _BTH );
	_managers.transform->SetTransform( _BTH, XMMatrixScaling( 1.0f, 1.0f, 1.0f ) );
	//System::GetCollision()->CreateBBT(_BTH);

	_anotherOne = _managers.entity.Create();
	_managers.mesh->CreateStaticMesh( _anotherOne, "Assets/Models/test.arf" );
	_managers.transform->CreateTransform( _anotherOne );
	_managers.transform->SetTransform( _anotherOne, XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation( 4.0f, 0.0f, 0.0f ) );
	_managers.transform->BindChild( _BTH, _anotherOne );

	_camera = _managers.entity.Create();

	_managers.camera->CreateCamera(_camera);
	_managers.transform->CreateTransform(_camera);
	//_transformManager->SetLookDir(_camera, XMVectorSet(0, 0, 1, 0));
	_managers.camera->SetActivePerspective(_camera);
	//_cameraView = XMMatrixLookAtLH(XMVectorSet(0, 0, -50, 1), XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0));
	//_cameraProj = XMMatrixPerspectiveFovLH(0.25f * XM_PI, 800.0f / 600.0f, 0.1f, 1000.0f);

	_managers.material->SetMaterialProperty(_BTH, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_managers.material->SetMaterialProperty(_anotherOne, 0, "Roughness", 0.15f, "Shaders/GBuffer.hlsl");
	_managers.material->SetTexture( _BTH, "DiffuseMap", L"Assets/Textures/bthcolor.dds" );
	_managers.material->SetTexture( _BTH, "DiffuseMap", L"Assets/Textures/bthcolor.dds", 1 );

	//_managers.material->SetFloat(_BTH, "Roughness", 0.90f, 0);

	
	
	_overlay = _managers.entity.Create();
	_managers.overlay->CreateOverlay(_overlay);
	_managers.transform->CreateTransform(_overlay);
	_managers.material->CreateMaterial(_overlay, "Shaders/GBuffer.hlsl");
	_managers.material->SetTexture(_overlay, "DiffuseMap", L"Assets/Textures/bthcolor.dds");
//	_managers.material->SetFloat(_overlay, "Roughness", 0.0f, 0); // TODO: Everything breaks when you add a material property. (No texture is assigned to the overlay.)
	_managers.transform->SetPosition(_overlay, XMVectorSet(0, 0, 0, 0));
	_managers.overlay->SetExtents(_overlay, 200, 200);

	//System::GetInput()->ToggleLockMouseToCenter();
	System::GetInput()->ToggleLockMouseToWindow();
	System::GetInput()->HideCursor(false);
}

void MenuState::Shutdown()
{
	State::Shutdown();
	//System::GetInput()->ToggleLockMouseToCenter();
	System::GetInput()->ToggleLockMouseToWindow();
	System::GetInput()->HideCursor(false);
}

void MenuState::HandleInput()
{
	if(System::GetInput()->IsKeyDown(VK_ESCAPE))
		throw FinishMsg(1);
	if (System::GetInput()->IsKeyDown(VK_W))
		_managers.transform->MoveForward(_camera, 10*_gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_S))
		_managers.transform->MoveBackward(_camera,10 *_gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_A))
		_managers.transform->MoveLeft(_camera, 10 * _gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_D))
		_managers.transform->MoveRight(_camera, 10 * _gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_SHIFT))
		_managers.transform->MoveUp(_camera, 10 * _gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_CONTROL))
		_managers.transform->MoveDown(_camera, 10 * _gameTimer.DeltaTime());

	if (System::GetInput()->GetKeyStateAndReset(VK_SPACE))
		System::GetInstance()->ToggleFullscreen();

	int x, y;
	System::GetInput()->GetMouseDiff(x, y);
	if(x!=0)
		_managers.transform->RotateYaw(_camera, x*_gameTimer.DeltaTime()*50);
	if(y!=0)
		_managers.transform->RotatePitch(_camera, y*_gameTimer.DeltaTime()*50);

	System::GetInput()->GetMousePos(x, y);
	_managers.transform->SetPosition(_overlay, XMVectorSet(x, y, 0, 0));

}

void MenuState::Update()
{
	_gameTimer.Tick();

	XMMATRIX transform = _managers.transform->GetTransform( _BTH );
	transform *= XMMatrixRotationY( 0.0167f * 0.1f * XM_PIDIV4 );
	_managers.transform->SetTransform( _BTH, transform );

	transform = _managers.transform->GetTransform( _anotherOne );
	transform = XMMatrixRotationY( 0.0167f * 0.4f * XM_PIDIV4 ) * transform;
	_managers.transform->SetTransform( _anotherOne, transform );
}

void MenuState::Render()
{
	System::GetGraphics()->Render( _gameTimer.TotalTime(), _gameTimer.DeltaTime() );
}