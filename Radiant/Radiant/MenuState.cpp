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

	_BTH = _managers->CreateObject(
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), 
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), 
		XMVectorSet(5.0f, 5.0f, 5.0f, 0.0f), 
		"Assets/Models/cube.arf", 
		"Assets/Textures/stonetex.dds", 
		"Assets/Textures/stonetexnormal.dds");
	_managers->material->SetMaterialProperty(_BTH, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");

	_anotherOne = _managers->CreateObject(
		XMVectorSet(500.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.5f, 0.5f, 0.5f, 0.0f),
		"Assets/Models/test.arf",
		"Assets/Textures/stonetexnormal.dds",
		"Assets/Textures/stonetexnormal.dds");
	_managers->material->SetMaterialProperty(_anotherOne, 1, "Roughness", 0.95f, "Shaders/GBuffer.hlsl");
	
	//_managers->transform->BindChild( _BTH, _anotherOne );

	_camera = _managers->CreateCamera(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		
	_overlay = _managers->CreateOverlay(
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
		200,
		200,
		"Assets/Textures/stonetex.dds");

	Entity test = _managers->CreateObject(
		XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.1f, 0.1f, 0.1f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/stonetex.dds",
		"Assets/Textures/stonetexnormal.dds");
	//_managers->transform->BindChild(_camera, test);

	_managers->camera->CreateCamera(_BTH);
	
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
	if (System::GetInput()->GetKeyStateAndReset(VK_C))
		_managers->camera->SetActivePerspective(_camera);
	if (System::GetInput()->GetKeyStateAndReset(VK_M))
		_managers->camera->SetActivePerspective(_BTH);

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
		_managers->transform->RotatePitch(_camera, x*_gameTimer.DeltaTime()*50);
	if(y!=0)
		_managers->transform->RotateRoll(_camera, y*_gameTimer.DeltaTime()*50);

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
	//_managers->transform->RotatePitch(_anotherOne, 100.0f *_gameTimer.DeltaTime());

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
