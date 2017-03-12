#include "APLState.h"
#include "System.h"
#include "Dungeon.h"

APLState::APLState()
{
}


APLState::~APLState()
{
	delete (Dungeon*)_dungeon;
}

void APLState::Init()
{	
	_BuildScene();
	_SetupInput();
}

void APLState::Shutdown()
{
}
const void APLState::_BuildScene()
{
	auto o = System::GetOptions();
	auto i = System::GetInput();
	float width = (float)o->GetScreenResolutionWidth();
	float height = (float)o->GetScreenResolutionHeight();
	float widthPercentOfDefault = (1.0f / 1920.0f) * width;
	float fontSize = 40 * widthPercentOfDefault;
	XMFLOAT4 TextColor = XMFLOAT4(41.0f / 255.0f, 127.0f / 255.0f, 185.0f / 255.0f, 1.0f);
	auto audio = Audio::GetInstance();


	/*********Generate a dungeon************/
	auto d = new Dungeon(25, 2, 2, 0.90f, _builder, 1);
	_dungeon = d;
	auto p = d->GetunoccupiedSpace();

	// Set camera stuff
	_camera = _builder->CreateCamera(XMVectorSet((float)p.x, 0.5f, (float)p.y, 1.0f));
	_controller->Camera()->SetActivePerspective(_camera);
	_builder->Bounding()->CreateBoundingSphere(_camera, 0.3f);

	const std::vector<Entity>& walls = d->GetWalls();
	const std::vector<Entity>& fr = d->GetFloorRoof();
	const std::vector<Entity>& pillars = d->GetPillars();

	std::vector<Entity> vect;
	vect.insert(vect.begin(), walls.begin(), walls.end());
	vect.insert(vect.begin(), fr.begin(), fr.end());
	vect.insert(vect.begin(), pillars.begin(), pillars.end());


	// Collision
	auto quadTree = _builder->EntityC().Create();
	_builder->Bounding()->CreateQuadTree(quadTree, vect);

	auto collisionUpdate = _builder->EntityC().Create();
	_controller->BindEventHandler(collisionUpdate, EventManager::Type::Overlay);
	_controller->BindEvent(collisionUpdate, EventManager::EventType::Update, [i, this, quadTree]()
	{
		_controller->Bounding()->GetMTV(quadTree, _camera,
			[this](DirectX::XMVECTOR& outMTV, const Entity& entity)
		{
			_controller->Transform()->MoveAlongVector(_camera, outMTV);

		});
	});


	/***********Create all the audio stuff*************/
	auto flashlight = _builder->EntityC().Create();
	_builder->Light()->BindSpotLight(flashlight, XMFLOAT3(1.0f, 1.0f, 1.0f), 0.45f, XM_PI / 2.8f, XM_PI / 4.8f, 10.0f);
	_builder->Light()->SetAsVolumetric(flashlight, false);
	_builder->Transform()->CreateTransform(flashlight);
	_builder->Transform()->SetPosition(flashlight, XMFLOAT3(0.5f, -0.5f, -0.3f));
	_builder->Transform()->BindChild(_camera, flashlight);

	auto ambMusic = _builder->EntityC().Create();
	_builder->Audio()->BindEntity(ambMusic);
	_builder->Audio()->AddAudio(ambMusic, "Audio/BGMusic/mamb.wav", AudioType::BG | AudioType::Looping);
	_builder->Audio()->StartAudio(ambMusic, 1.0f);


	auto light1 = _builder->EntityC().Create();
	p = d->GetunoccupiedSpace();
	_builder->Light()->BindPointLight(light1, XMFLOAT3(p.x, 0.5f, p.y), 3.0f, XMFLOAT3(1.0f, 0.0f, 0.0f), 0.45f);
	_builder->Light()->ChangeLightBlobRange(light1, 1.0f);
	_builder->Transform()->CreateTransform(light1);	
	_builder->Audio()->BindEntity(light1);
	_builder->Audio()->AddAudio(light1, "Audio/SoundEffects/pew.wav", AudioType::Effect | AudioType::Sterio_Pan | AudioType::Looping);
	_builder->Audio()->StartAudio(light1, 0.4f);
	_builder->Transform()->SetPosition(light1, XMFLOAT3(p.x, 0.5f, p.y));

	light1 = _builder->EntityC().Create();
	p = d->GetunoccupiedSpace();
	_builder->Light()->BindPointLight(light1, XMFLOAT3(p.x, 0.5f, p.y), 3.0f, XMFLOAT3(0.0f, 0.0f, 1.0f), 0.45f);
	_builder->Light()->ChangeLightBlobRange(light1, 1.0f);
	_builder->Transform()->CreateTransform(light1);
	_builder->Audio()->BindEntity(light1);
	_builder->Audio()->AddAudio(light1, "Audio/SoundEffects/teleport.wav", AudioType::Effect | AudioType::Sterio_Pan | AudioType::Looping);
	_builder->Audio()->StartAudio(light1, 0.4f);
	_builder->Transform()->SetPosition(light1, XMFLOAT3(p.x, 0.5f, p.y));

	light1 = _builder->EntityC().Create();
	p = d->GetunoccupiedSpace();
	_builder->Light()->BindPointLight(light1, XMFLOAT3(p.x, 0.5f, p.y), 3.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), 0.45f);
	_builder->Light()->ChangeLightBlobRange(light1, 1.0f);
	_builder->Transform()->CreateTransform(light1);
	_builder->Audio()->BindEntity(light1);
	_builder->Audio()->AddAudio(light1, "Audio/SoundEffects/blabla.flac", AudioType::Effect | AudioType::Sterio_Pan | AudioType::Looping | AudioType::Radio);
	_builder->Audio()->StartAudio(light1, 1.5f);
	_builder->Transform()->SetPosition(light1, XMFLOAT3(p.x, 0.5f, p.y));


	// Bind footstep to camera
	_builder->Audio()->BindEntity(_camera);
	_builder->Audio()->AddAudio(_camera, "Audio/SoundEffects/concfootstep.wav", AudioType::Effect | AudioType::On_Move | AudioType::Looping);
	_builder->Audio()->StartAudio(_camera, 1.0f);
}
const void APLState::_SetupInput()
{
	auto i = System::GetInput();
	auto inputUpdateEnt = _builder->EntityC().Create();
	_controller->BindEventHandler(inputUpdateEnt, EventManager::Type::Overlay);
	_controller->BindEvent(inputUpdateEnt, EventManager::EventType::Update, [i, this]()
	{
		if (i->IsKeyPushed(VK_ESCAPE))
		{
			ExitApplication;
		}

		auto deltatime = _gameTimer.DeltaTime();
		auto i = System::GetInput();

		int x, y;

		XMVECTOR moveVec = XMVectorZero();
		XMVECTOR forward = _builder->GetEntityController()->Transform()->GetDirection(_camera);
		XMVECTOR right = _builder->GetEntityController()->Transform()->GetRight(_camera);
		XMVECTOR up = _builder->GetEntityController()->Transform()->GetUp(_camera);
		bool change = false;
		if (i->IsMouseKeyDown(VK_LBUTTON))
		{
			i->GetMouseDiff(x, y);
			if (x != 0)
				_builder->GetEntityController()->Transform()->RotateYaw(_camera, x  * 0.1f);
			if (y != 0)
				_builder->GetEntityController()->Transform()->RotatePitch(_camera, y  * 0.1f);
		}



		if (i->IsKeyDown(VK_W))
		{
			moveVec += forward;
			change = true;
		}
		if (i->IsKeyDown(VK_S))
		{
			moveVec -= forward;
			change = true;
		}
		if (i->IsKeyDown(VK_A))
		{
			moveVec -= right;
			change = true;
		}
		if (i->IsKeyDown(VK_D))
		{
			moveVec += right;
			change = true;
		}

		if (change)
		{

			_builder->GetEntityController()->Transform()->MoveAlongVector(_camera, XMVector3Normalize(moveVec), 1.0*deltatime);

		}



	}
	);

}
//
//void APLState::Update()
//{
//	
//
//
//
//	//static uint framecount = 10;
//	//framecount++;
//	//if (framecount > 2)
//	//{
//	//	std::vector<Entity> entites;
//	//	_controller->Bounding()->GetEntitiesInFrustumNoQuadTree(_controller->Camera()->GetFrustum(_camera), entites);
//	//		_controller->Light()->SetInFrustum(entites);
//	//	_controller->Bounding()->GetEntitiesInFrustum(_controller->Camera()->GetFrustum(_camera), entites);
//	//	_controller->Mesh()->SetInFrustum(entites);
//	//	framecount = 0;
//	//}
//}
//
//void APLState::Render()
//{
//	System::GetGraphics()->Render(_gameTimer.TotalTime(), _gameTimer.DeltaTime());
//}
