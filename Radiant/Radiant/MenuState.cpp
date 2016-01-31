#include "MenuState.h"
#include "System.h"
#include "Graphics.h"

using namespace DirectX;

MenuState::MenuState() : State()
{
	_managers = nullptr;
	try{_managers = new ManagerWrapper;}
	catch (std::exception& e) { e; throw ErrorMsg(3000002, L"Failed to create managerwrapper."); }
	_passed = false;
}

MenuState::MenuState(ManagerWrapper* wrapper) : _managers(wrapper)
{
}


MenuState::~MenuState()
{
	//SAFE_DELETE(_managers);
}


void MenuState::Init()
{
	//System::GetInstance()->ToggleFullscreen();

	_BTH = _managers->entity.Create();
	_managers->mesh->CreateStaticMesh( _BTH, "Assets/Models/cube.arf" );
	_managers->transform->CreateTransform( _BTH );
	_managers->transform->SetScale(_anotherOne, XMVectorSet(5.0f, 5.0f, 5.0f, 0.0f));
	//System::GetCollision()->CreateBBT(_BTH);

	_anotherOne = _managers->entity.Create();
	_managers->mesh->CreateStaticMesh( _anotherOne, "Assets/Models/test.arf" );
	_managers->transform->CreateTransform( _anotherOne );
	_managers->transform->SetScale(_anotherOne, XMVectorSet(0.5f, 0.5f, 0.5f, 0.0f));
	_managers->transform->SetPosition(_anotherOne, XMVectorSet(10.0f, 0.0f, 0.0f, 0.0f));
	_managers->transform->BindChild( _BTH, _anotherOne );

	_camera = _managers->entity.Create();

	_managers->camera->CreateCamera(_camera);
	_managers->transform->CreateTransform(_camera);
	_managers->transform->SetFlyMode(_camera, false);
	//_transformManager->SetLookDir(_camera, XMVectorSet(0, 0, 1, 0));
	_managers->camera->SetActivePerspective(_camera);

	//_cameraView = XMMatrixLookAtLH(XMVectorSet(0, 0, -50, 1), XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0));
	//_cameraProj = XMMatrixPerspectiveFovLH(0.25f * XM_PI, 800.0f / 600.0f, 0.1f, 1000.0f);
	
	_managers->material->BindMaterial(_BTH, "Shaders/GBuffer.hlsl");
	_managers->material->SetMaterialProperty(_BTH, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_managers->material->SetSubMeshTexture(_BTH, "DiffuseMap", L"Assets/Textures/stonetex.dds", 0);
	_managers->material->SetSubMeshTexture(_BTH, "NormalMap", L"Assets/Textures/stonetexnormal.dds", 0);

	_managers->material->BindMaterial(_anotherOne, "Shaders/GBuffer.hlsl");
	//_managers->material->SetMaterialProperty(_anotherOne, 1, "Roughness", 0.95f, "Shaders/GBuffer.hlsl");
	_managers->material->SetEntityTexture(_anotherOne, "NormalMap", L"Assets/Textures/stonetexnormal.dds");
	_managers->material->SetEntityTexture(_anotherOne, "DiffuseMap", L"Assets/Textures/stonetexnormal.dds");
	_managers->material->SetMaterialProperty(_anotherOne, 1, "Roughness", 0.95f, "Shaders/GBuffer.hlsl");
	/*_managers->material->SetMaterialProperty(_anotherOne, 1, "Roughness", 0.95f, "Shaders/GBuffer.hlsl");
	_managers->material->SetMaterialProperty(_anotherOne, 2, "Roughness", 0.95f, "Shaders/GBuffer.hlsl");*/
	

		
	_overlay = _managers->entity.Create();
	_managers->material->BindMaterial(_overlay, "Shaders/GBuffer.hlsl");
	_managers->overlay->CreateOverlay(_overlay);
	_managers->transform->CreateTransform(_overlay);
	_managers->material->SetEntityTexture(_overlay, "DiffuseMap", L"Assets/Textures/stonetex.dds");
	_managers->transform->SetPosition(_overlay, XMVectorSet(0, 0, 0, 0));
	_managers->overlay->SetExtents(_overlay, 200, 200);

	System::GetInput()->LockMouseToCenter(true);
	System::GetInput()->LockMouseToWindow(true);
	System::GetInput()->HideCursor(true);
}

void MenuState::Shutdown()
{
	State::Shutdown();
	if(!_passed)
		DeleteManager();

	System::GetInput()->LockMouseToCenter(false);
	System::GetInput()->LockMouseToWindow(false);
	System::GetInput()->HideCursor(false);

}

void MenuState::HandleInput()
{
	if(System::GetInput()->IsKeyDown(VK_ESCAPE))
		throw FinishMsg(1);
	if (System::GetInput()->IsKeyDown(VK_W))
		_managers->transform->MoveForward(_camera, 10*_gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_S))
		_managers->transform->MoveBackward(_camera,10 *_gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_A))
		_managers->transform->MoveLeft(_camera, 10 * _gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_D))
		_managers->transform->MoveRight(_camera, 10 * _gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_SHIFT))
		_managers->transform->MoveUp(_camera, 10 * _gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_CONTROL))
		_managers->transform->MoveDown(_camera, 10 * _gameTimer.DeltaTime());

	if (System::GetInput()->IsKeyDown(VK_O))
	{
		float inc = _managers->material->GetMaterialPropertyOfSubMesh(_BTH, "Roughness", 0);
		inc += _gameTimer.DeltaTime() * 2;
		if (inc > 1.0f)
			inc = 1.0f;
		_managers->material->SetMaterialProperty(_BTH, 0, "Roughness", inc, "Shaders/GBuffer.hlsl");
	}

	if (System::GetInput()->IsKeyDown(VK_P))
	{
		float inc = _managers->material->GetMaterialPropertyOfSubMesh(_BTH, "Roughness", 0);
		inc -= _gameTimer.DeltaTime() * 2;
		if (inc < 0.0f)
			inc = 0.0f;
		_managers->material->SetMaterialProperty(_BTH, 0, "Roughness", inc, "Shaders/GBuffer.hlsl");
	}

	if (System::GetInput()->GetKeyStateAndReset(VK_SPACE))
		System::GetInstance()->ToggleFullscreen();

	int x, y;
	System::GetInput()->GetMouseDiff(x, y);
	if(x!=0)
		_managers->transform->RotateYaw(_camera, x*_gameTimer.DeltaTime()*50);
	if(y!=0)
		_managers->transform->RotatePitch(_camera, y*_gameTimer.DeltaTime()*50);

	System::GetInput()->GetMousePos(x, y);
	_managers->transform->SetPosition(_overlay, XMVectorSet(x, y, 0, 0));

}

void MenuState::Update()
{
	_gameTimer.Tick();

	/*XMMATRIX transform = _managers->transform->GetTransform( _BTH );
	transform *= XMMatrixRotationY( 0.0167f * 0.1f * XM_PIDIV4 );
	_managers->transform->SetTransform( _BTH, transform );

	transform = _managers->transform->GetTransform( _anotherOne );
	transform = XMMatrixRotationY( 0.0167f * 0.4f * XM_PIDIV4 ) * transform;
	_managers->transform->SetTransform( _anotherOne, transform );*/

	_managers->transform->RotatePitch(_BTH, 40.0f *_gameTimer.DeltaTime());
	_managers->transform->RotatePitch(_anotherOne, 100.0f *_gameTimer.DeltaTime());

	//System::GetFileHandler()->DumpToFile( "Test line" + to_string(_gameTimer.DeltaTime()));
}

void MenuState::Render()
{
	System::GetGraphics()->Render( _gameTimer.TotalTime(), _gameTimer.DeltaTime() );
}


const void MenuState::DeleteManager()
{
	SAFE_DELETE(_managers);
}
