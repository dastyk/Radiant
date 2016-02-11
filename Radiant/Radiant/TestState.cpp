#include "TestState.h"
#include "System.h"
#include "Graphics.h"
#include "Audio.h"

using namespace DirectX;



TestState::TestState() : State()
{
	
}

TestState::~TestState()
{
}


void TestState::Init()
{
	_controller->SetExclusiveRenderAccess();
	//System::GetInstance()->ToggleFullscreen();

	_BTH = _builder->CreateObject(
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), 
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), 
		XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		"Assets/Models/cube.arf", 
		"Assets/Textures/ft_stone01_c.png",
		"Assets/Textures/ft_stone01_n.png" );

	_point = _builder->CreateObject(
		XMVectorSet(2.0f, -1.0f, 0.0f, 1.0f), 
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), 
		XMVectorSet(0.10f, 0.10f, 0.10f, 0.0f), 
		"Assets/Models/cube.arf", 
		"Assets/Textures/stonetex.dds", 
		"Assets/Textures/stonetexnormal.dds");

	_controller->Light()->BindPointLight(_point, XMFLOAT3(50.0f, 10.0f, 50.0f), 100.0f, XMFLOAT3(1.0f, 1.0f, 1.0f), 5.0f);
	_controller->Transform()->CreateTransform(_point);
	_controller->Transform()->SetPosition(_point, XMVectorSet(0.0f, 10.0f, 0.0f, 0.0f));
	_controller->Transform()->MoveUp(_point, 5.0f);
	_controller->Material()->SetMaterialProperty(_BTH, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");

//	_controller->Bounding()->CreateBoundingBox(_point, _controller->Mesh()->GetMesh(_point));
	test = _builder->CreateObject(
		XMVectorSet(0.0f, 0.0f, 15.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/ft_stone01_c.png",
		"Assets/Textures/ft_stone01_n.png");
	_controller->Transform()->BindChild(_BTH, test);

	_anotherOne = _builder->CreateObject(
		XMVectorSet(0.0f, 0.0f, 10.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/ft_stone01_c.png",
		"Assets/Textures/ft_stone01_n.png");
	//_controller->Material()->SetMaterialProperty(_anotherOne, 0, "Roughness", 0.95f, "Shaders/GBuffer.hlsl");

	//_controller->Material()->SetMaterialProperty(_anotherOne, 1, "Roughness", 0.95f, "Shaders/GBuffer.hlsl");
	//_controller->Material()->SetSubMeshTexture(_anotherOne, "DiffuseMap", L"Assets/Textures/stonetex.dds", 1);


	_controller->Transform()->BindChild(test, _anotherOne );

	_camera = _builder->CreateCamera(XMVectorSet(0.0f, 0.0f, -20.0f, 0.0f));
	//_controller->Light()->BindPointLight(_camera, XMFLOAT3(0.0f, 5.0f, 0.0f), 5.0f, XMFLOAT3(1.0f, 1.0f, 1.0f), 10.0f);
	//_controller->Light()->SetAsVolumetric(_camera, false);

	//_overlay = _builder->CreateOverlay(
	//	XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
	//	200,
	//	200,
	//	"Assets/Textures/stonetex.dds");

	_areaRectLight = _builder->CreateObject(
		XMVectorSet(-30.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(6.0f, 6.0f, 6.0f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/color_map.png",
		"Assets/Textures/normal_map.png");
	_controller->Material()->SetEntityTexture(_areaRectLight, "DisplacementMap", L"Assets/Textures/height_map.png");

	//Yeah we need something that sets the material for the entire entity
	_controller->Material()->SetMaterialProperty(_areaRectLight, "ParallaxScaling", 0.04f, "Shaders/GBuffer.hlsl");
	_controller->Material()->SetMaterialProperty(_areaRectLight, "ParallaxBias", -0.03f, "Shaders/GBuffer.hlsl");
	

	_controller->Material()->SetMaterialProperty(_areaRectLight, "Roughness", 0.3f, "Shaders/GBuffer.hlsl");
	_controller->Material()->SetMaterialProperty(_areaRectLight, "Metallic", 0.9f, "Shaders/GBuffer.hlsl");
	//
	//
	//_controller->Light()->BindPointLight(_areaRectLight, XMFLOAT3(-30.0f, 10.0f, -10.0f), 25.0f, XMFLOAT3(0.0f, 1.0f, 0.4f), 24.0f);
	//_controller->Light()->BindAreaRectLight(_areaRectLight, XMFLOAT3(-25.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), 10.0f, XMFLOAT3(0.0f, 0.0f, 1.0f), 3.0f, 0.05f, XMFLOAT3(0.0f, 1.0f, 1.0f), 48.0f);



	Entity tost = _builder->CreateObject(
		XMVectorSet(0.0f, 0.0f, 40.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/default_color.png"
		);

	//_controller->Camera()->CreateCamera(_BTH);
	
	test2 = _builder->CreateObject(
		XMVectorSet(1.5f, -0.2f, 1.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/ft_stone01_c.png",
		"Assets/Textures/ft_stone01_n.png");
	_controller->Transform()->BindChild(_camera, test2);
//	_controller->Bounding()->CreateBoundingBox(test2, _controller->Mesh()->GetMesh(_BTH));
	_controller->Material()->SetMaterialProperty( test2, 0, "Roughness", 0.1f, "Shaders/GBuffer.hlsl" );


	//Entity ar = _builder->entity.Create();
	//_controller->Light()->BindAreaRectLight(ar, XMFLOAT3(-2.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), 25.0f, XMFLOAT3(0.0f, 0.0f, 1.0f), 20.0f, 0.05f, XMFLOAT3(0.0f, 0.0f, 1.0f), 20.0f);


	_controller->BindEventHandler(test2, EventManager::Type::Overlay);
	_controller->BindEvent(test2,
		EventManager::EventType::Update,
		[this]()
	{
		HandleInput();
	});



	System::GetInput()->LockMouseToCenter(true);
	System::GetInput()->LockMouseToWindow(true);
	System::GetInput()->HideCursor(true);
}

void TestState::Shutdown()
{

	System::GetInput()->LockMouseToCenter(false);
	System::GetInput()->LockMouseToWindow(false);
	System::GetInput()->HideCursor(false);

}

void TestState::HandleInput()
{
	_timer.TimeStart("Input");

	if (System::GetInput()->IsKeyPushed(VK_T))
	{
		_controller->ReleaseEntity(_areaRectLight);
		//_controller->Mesh()->ReleaseMesh(_areaRectLight);
		
	}

	if(System::GetInput()->IsKeyPushed(VK_ESCAPE))
	{ 
		System::GetInput()->LockMouseToCenter(false);
		System::GetInput()->LockMouseToWindow(false);
		System::GetInput()->HideCursor(false);
		ChangeStateTo(StateChange(new MenuState));
	}
	if (System::GetInput()->IsKeyDown(VK_W))
		_controller->Transform()->MoveForward(_camera, 10*1000*_gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_S))
		_controller->Transform()->MoveBackward(_camera,10 * 1000 *_gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_A))
		_controller->Transform()->MoveLeft(_camera, 10 * 1000 * _gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_D))
		_controller->Transform()->MoveRight(_camera, 10 * 1000 * _gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_SHIFT))
		_controller->Transform()->MoveUp(_camera, 10 * 1000 * _gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyDown(VK_CONTROL))
		_controller->Transform()->MoveDown(_camera, 10 * 1000 * _gameTimer.DeltaTime());
	if (System::GetInput()->IsKeyPushed(VK_C))
		_controller->Camera()->SetActivePerspective(_camera);
	if (System::GetInput()->IsKeyPushed(VK_M))
		_controller->Camera()->SetActivePerspective(_BTH);
	/*if (System::GetInput()->IsKeyPushed(VK_H))
		_builder->text->ChangeText(_overlay, "Test2");*/
	if (System::GetInput()->IsKeyDown(VK_O))
	{
		float inc = _controller->Material()->GetMaterialPropertyOfSubMesh(test2, "Roughness", 0);
		inc += _gameTimer.DeltaTime() * 2;
		if (inc > 1.0f)
			inc = 1.0f;
		_controller->Material()->SetMaterialProperty(test2, 0, "Roughness", inc, "Shaders/GBuffer.hlsl");
	}

	if (System::GetInput()->IsKeyDown(VK_P))
	{
		float inc = _controller->Material()->GetMaterialPropertyOfSubMesh(test2, "Roughness", 0);
		inc -= _gameTimer.DeltaTime() * 2;
		if (inc < 0.0f)
			inc = 0.0f;
		_controller->Material()->SetMaterialProperty(test2, 0, "Roughness", inc, "Shaders/GBuffer.hlsl");
	}

	if ( System::GetInput()->IsKeyDown( VK_U ) )
	{
		float inc = _controller->Material()->GetMaterialPropertyOfSubMesh(test2, "Metallic", 0 );
		inc += _gameTimer.DeltaTime() * 2;
		if ( inc > 1.0f )
			inc = 1.0f;
		_controller->Material()->SetMaterialProperty(test2, 0, "Metallic", inc, "Shaders/GBuffer.hlsl" );
	}

	if ( System::GetInput()->IsKeyDown( VK_I ) )
	{
		float inc = _controller->Material()->GetMaterialPropertyOfSubMesh(test2, "Metallic", 0 );
		inc -= _gameTimer.DeltaTime() * 2;
		if ( inc < 0.0f )
			inc = 0.0f;
		_controller->Material()->SetMaterialProperty(test2, 0, "Metallic", inc, "Shaders/GBuffer.hlsl" );
	}

	if (System::GetInput()->IsKeyDown(VK_M))
	{
		float inc = _controller->Material()->GetMaterialPropertyOfEntity(_areaRectLight, "ParallaxScaling");
		inc += _gameTimer.DeltaTime() * 10.0f;
		_controller->Material()->SetMaterialProperty(_areaRectLight, "ParallaxScaling", inc, "Shaders/GBuffer.hlsl");
	}

	if (System::GetInput()->IsKeyDown(VK_N))
	{
		float inc = _controller->Material()->GetMaterialPropertyOfEntity(_areaRectLight, "ParallaxScaling");
		inc -= _gameTimer.DeltaTime() * 10.0f;
		_controller->Material()->SetMaterialProperty(_areaRectLight, "ParallaxScaling", inc, "Shaders/GBuffer.hlsl");
	}

	if (System::GetInput()->IsKeyPushed(VK_SPACE))
		System::GetInstance()->ToggleFullscreen();

	int x, y;
	System::GetInput()->GetMouseDiff(x, y);
	if(x!=0)
		_controller->Transform()->RotateYaw(_camera, x*0.1f);
	if(y!=0)
		_controller->Transform()->RotatePitch(_camera, y*0.1f);
	//System::GetInput()->GetMousePos(x, y);
	//if(System::GetInput()->IsMouseKeyDown(VK_LBUTTON))
	//	if(_builder->clicking->IsClicked(_overlay))
	//		throw FinishMsg(1);
	//_controller->Transform()->SetPosition(_overlay, XMVectorSet(static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f));
	_timer.TimeEnd("Input");
}

void TestState::Update()
{
	State::Update();
	//HandleInput();
	_timer.TimeStart("Update");
	_gameTimer.Tick();

	_controller->Transform()->RotateYaw(_BTH, 10.0f *_gameTimer.DeltaTime());
	_controller->Transform()->RotateYaw(test, 80.0f *_gameTimer.DeltaTime());
	_controller->Transform()->RotateYaw(_anotherOne, 40.0f *_gameTimer.DeltaTime());


	//System::GetFileHandler()->DumpToFile("Test line" + to_string(_gameTimer.DeltaTime()));

	if (System::GetInstance()->GetInput()->IsKeyPushed('L'))
		System::GetInstance()->GetAudio()->PlaySoundEffect(L"test.wav", 1);

	//if (System::GetInput()->IsKeyDown(VK_K))
	//	if (_controller->Bounding()->CheckCollision(_point, test2))
	//		throw FinishMsg(1);
	_timer.TimeEnd("Update");
	_timer.GetTime();
}

void TestState::Render()
{
	System::GetGraphics()->Render( _gameTimer.TotalTime(), _gameTimer.DeltaTime() );
}
