#include "MenuState.h"
#include "System.h"
#include "Graphics.h"
#include "Audio.h"

using namespace DirectX;

MenuState::MenuState() : State()
{
	_managers = nullptr;
	try{_managers = new ManagerWrapper;}
	catch (std::exception& e) { e; throw ErrorMsg(3000002, L"Failed to create managerWrapper in the MenuState."); }
	_passed = false;

	_managers->SetExclusiveRenderAccess();
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

	State::Init();

	Entity wrapper = _managers->entity.Create();
	_managers->transform->CreateTransform( wrapper );

	_BTH = _managers->CreateObject(
		XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f ),
		XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f ),
		XMVectorSet( 1.0f, 1.0f, 1.0f, 0.0f ),
		//"Assets/Models/cube.arf", 
		"Assets/Models/test.arf",
		//"Assets/Textures/ft_stone01_c.png",
		"Assets/Textures/bthcolor.dds",
		"Assets/Textures/ft_stone01_n.png" );
	//_managers->transform->SetScale( _BTH, XMVectorSet( 0.1f, 0.1f, 0.1f, 1 ) );
	_managers->mesh->Hide( _BTH, 0 );
	_managers->transform->BindChild( wrapper, _BTH );

	_BTH2 = _managers->CreateObject(
		XMVectorSet( 0, 0, 0, 0 ),
		XMVectorSet( 0, 0, 0, 0 ),
		XMVectorSet( 1.0f, 1.0f, 1.0f, 1.0f ),
		"Assets/Models/bth.arf",
		"Assets/Textures/bthcolor.dds",
		"Assets/Textures/ft_stone01_n.png" );
	_managers->transform->SetScale( _BTH2, XMVectorSet( 0.1f, 0.1f, 0.1f, 1 ) );
	_managers->transform->BindChild( wrapper, _BTH2 );
	_managers->mesh->Hide( _BTH2, 0 );
	_managers->mesh->Hide( _BTH2, 1 );

	_point = _managers->CreateObject(XMVectorSet(2.0f, -1.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.10f, 0.10f, 0.10f, 0.0f), "Assets/Models/cube.arf", "Assets/Textures/stonetex.dds", "Assets/Textures/stonetexnormal.dds");
	_managers->light->BindPointLight(_point, XMFLOAT3(50.0f, 10.0f, 50.0f), 100.0f, XMFLOAT3(1.0f, 1.0f, 1.0f), 5.0f);
	_managers->transform->CreateTransform(_point);
	_managers->transform->SetPosition(_point, XMVectorSet(0.0f, 10.0f, 0.0f, 0.0f));
	_managers->transform->MoveUp(_point, 5.0f);
	_managers->material->SetMaterialProperty(_BTH, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");

	_managers->bounding->CreateBoundingBox(_point, _managers->mesh->GetMesh(_point));
	Entity test = _managers->CreateObject(
		XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/stonetex.dds",
		"Assets/Textures/stonetexnormal.dds");
	_managers->transform->BindChild(_BTH, test);
	_managers->transform->SetScale( test, XMVectorSet( 10.0f, 10.0f, 10.0f, 1 ) );

	_anotherOne = _managers->CreateObject(
		XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		"Assets/Models/test.arf",
		"Assets/Textures/stonetexnormal.dds",
		"Assets/Textures/stonetexnormal.dds");
	_managers->material->SetMaterialProperty(_anotherOne, 0, "Roughness", 0.95f, "Shaders/GBuffer.hlsl");

	_managers->material->SetMaterialProperty(_anotherOne, 1, "Roughness", 0.95f, "Shaders/GBuffer.hlsl");
	_managers->material->SetSubMeshTexture(_anotherOne, "DiffuseMap", L"Assets/Textures/stonetex.dds", 1);


	_managers->transform->BindChild(test, _anotherOne );

	_camera = _managers->CreateCamera(XMVectorSet(0.0f, 0.0f, -20.0f, 0.0f));
	_managers->light->BindPointLight(_camera, XMFLOAT3(0.0f, 5.0f, 0.0f), 5.0f, XMFLOAT3(1.0f, 1.0f, 1.0f), 10.0f);
	_managers->light->BindSpotLight(_camera, XMFLOAT3(1.0f, 0.0f, 0.0f), 10.0f, XMConvertToRadians( 40.0f ), XMConvertToRadians( 20.0f ), 10.0f);

	_overlay = _managers->CreateOverlay(
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
		200,
		200,
		"Assets/Textures/stonetex.dds");
	_managers->clicking->BindOverlay(_overlay);
	_managers->overlay->SetExtents(_overlay, 200, 200);
	_managers->transform->SetPosition(_overlay, XMVectorSet(0.0, 0.0, 0.0, 0.0));
	_managers->text->BindText(_overlay, "Test", "Assets/Fonts/cooper", 40, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	
	Entity o2 = _managers->CreateOverlay(
		XMVectorSet(5.0f, 5.0f, 0.0f, 0.0f),
		50,
		50,
		"Assets/Textures/stonetexnormal.dds");
	_managers->transform->BindChild(_overlay, o2);


	

	_managers->camera->CreateCamera(_BTH);
	
	test2 = _managers->CreateObject(
		XMVectorSet(1.5f, -0.2f, 1.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/ft_stone01_c.png",
		"Assets/Textures/ft_stone01_n.png");
	_managers->transform->BindChild(_camera, test2);
	_managers->bounding->CreateBoundingBox(test2, _managers->mesh->GetMesh(_BTH));
	_managers->material->SetMaterialProperty( test2, 0, "Roughness", 0.1f, "Shaders/GBuffer.hlsl" );


	Entity ar = _managers->entity.Create();
	_managers->light->BindAreaRectLight(ar, XMFLOAT3(-2.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), 25.0f, XMFLOAT3(0.0f, 0.0f, 1.0f), 20.0f, 0.05f, XMFLOAT3(0.0f, 0.0f, 1.0f), 20.0f);

	//bounding->CreateBoundingBox(ent);
	//_managers->transform->SetPosition(map, XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	//_managers->transform->SetRotation(ent, rot);
	//_managers->transform->SetScale(ent, XMVectorSet(1.0f,1.0f,);
	System::GetInput()->LockMouseToCenter(true);
	System::GetInput()->LockMouseToWindow(true);
	System::GetInput()->HideCursor(false);
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
	if(System::GetInput()->GetKeyStateAndReset(VK_ESCAPE))
		throw FinishMsg(1);
	if (System::GetInput()->GetKeyStateAndReset(VK_F1))
		throw StateChange(new GameState, true);
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
	if (System::GetInput()->GetKeyStateAndReset(VK_H))
		_managers->text->ChangeText(_overlay, "Test2");
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

	if ( System::GetInput()->IsKeyDown( VK_U ) )
	{
		float inc = _managers->material->GetMaterialPropertyOfSubMesh( _BTH, "Metallic", 0 );
		inc += _gameTimer.DeltaTime() * 2;
		if ( inc > 1.0f )
			inc = 1.0f;
		_managers->material->SetMaterialProperty( _BTH, 0, "Metallic", inc, "Shaders/GBuffer.hlsl" );
	}

	if ( System::GetInput()->IsKeyDown( VK_I ) )
	{
		float inc = _managers->material->GetMaterialPropertyOfSubMesh( _BTH, "Metallic", 0 );
		inc -= _gameTimer.DeltaTime() * 2;
		if ( inc < 0.0f )
			inc = 0.0f;
		_managers->material->SetMaterialProperty( _BTH, 0, "Metallic", inc, "Shaders/GBuffer.hlsl" );
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
	if(System::GetInput()->IsMouseKeyDown(VK_LBUTTON))
		if(_managers->clicking->IsClicked(_overlay))
			throw FinishMsg(1);
	_managers->transform->SetPosition(_overlay, XMVectorSet(static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f));

}

void MenuState::Update()
{
	_timer.TimeStart("Update");
	_gameTimer.Tick();

	//float speed = 60.0f;
	float speed = 0;
	_managers->transform->RotateYaw(_BTH, speed *_gameTimer.DeltaTime());
	_managers->transform->RotateYaw( _BTH2, -speed * _gameTimer.DeltaTime() );
	_managers->transform->RotatePitch( _BTH2, speed * _gameTimer.DeltaTime() );
	_managers->transform->RotateYaw(_anotherOne, 40.0f *_gameTimer.DeltaTime());

	//static float temp = 0.0f;
	//temp += _gameTimer.DeltaTime();
	//if ( temp > 1.0f )
	//{
	//	temp -= 1.0f;
	//	_managers->mesh->ToggleVisibility( _BTH, 0 );
	//}

	//System::GetFileHandler()->DumpToFile("Test line" + to_string(_gameTimer.DeltaTime()));

	if (System::GetInstance()->GetInput()->GetKeyStateAndReset('L'))
		System::GetInstance()->GetAudio()->PlaySoundEffect(L"test.wav", 1);

	if (System::GetInput()->IsKeyDown(VK_U))
		if (_managers->bounding->CheckCollision(_point, test2))
			throw FinishMsg(1);
	_timer.TimeEnd("Update");
	_timer.GetTime();
}

void MenuState::Render()
{
	System::GetGraphics()->Render( _gameTimer.TotalTime(), _gameTimer.DeltaTime() );
}


const void MenuState::DeleteManager()
{
	SAFE_DELETE(_managers);
}