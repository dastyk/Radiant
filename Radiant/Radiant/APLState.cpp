#include "APLState.h"
#include <Audio.h>
#include "System.h"
struct Data
{
	bool run;
	Audio::FileInfo info;
};
CallbackPrototype(callback)
{
	static uint32_t count = 0;
	float* out = (float*)outputBuffer;
	size_t offset = count*framesPerBuffer;
	if (offset > fileInfo.info.frames)
	{
		count = 0;
		return paComplete;
	}
	for (unsigned long i = 0; i < framesPerBuffer; i++)
	{
		if (offset + i < fileInfo.info.frames)
		{


			*out++ = fileInfo.data[offset + i] * (1.0f - (offset / (float)fileInfo.info.frames));
			*out++ = fileInfo.data[offset + i] * (offset / (float)fileInfo.info.frames);
		}
		else
		{
			*out++ = 0.0f;
			*out++ = 0.0f;
		}
	}
	//	memcpy(outputbuffer, (float*)data.info.data + offset, sizeof(float)*framesperbuffer);
	count++;
	return paContinue;
}
FinishedCallbackPrototype(fStopCallback)
{
	//*(bool*)userData = false;	
	//((Audio*)userData)->StartStream(1, fCallback, userData, false);
	return paComplete;

}
FinishedCallbackPrototype(fLoopCallback)
{
	//*(bool*)userData = false;	
	//((Audio*)userData)->StartStream(1, fCallback, userData, false);
	return paContinue;

}
APLState::APLState()
{
}


APLState::~APLState()
{
}

void APLState::Init()
{
	auto o = System::GetOptions();
	auto i = System::GetInput();
	float width = (float)o->GetScreenResolutionWidth();
	float height = (float)o->GetScreenResolutionHeight();
	float widthPercentOfDefault = (1.0f / 1920.0f) * width;
	float fontSize = 40 * widthPercentOfDefault;
	XMFLOAT4 TextColor = XMFLOAT4(41.0f / 255.0f, 127.0f / 255.0f, 185.0f / 255.0f, 1.0f);
	auto audio = Audio::GetInstance();


	_camera = _builder->CreateCamera(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));

	_controller->Camera()->SetActivePerspective(_camera);

	Entity floor = _builder->CreateObject(
		XMVectorSet(0.0f, -1.0f, 5.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(100.0f, 1.0f, 100.0f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/Dungeon/0/Floor_Dif.png",
		"Assets/Textures/Dungeon/0/Floor_NM.png",
		"Assets/Textures/Dungeon/0/Floor_Disp.png",
		"Assets/Textures/Dungeon/0/Floor_Roughness.png",
		"Assets/Textures/Dungeon/0/Floor_Glossiness.png");

	_builder->Material()->SetMaterialProperty(floor, "TexCoordScaleU", 100.0f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(floor, "TexCoordScaleV", 100.0f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(floor, "ParallaxBias", -0.05f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(floor, "ParallaxScaling", 0.12f, "Shaders/GBuffer.hlsl");

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

			_builder->GetEntityController()->Transform()->MoveAlongVector(_camera, XMVector3Normalize(moveVec), 0.5*deltatime);

		}

	}
	);

	//auto& info = audio->ReadFile("Audio/BGMusic/mamb.wav");
	//info.info.channels = 2;
	////info.info.samplerate *= 4;
	//audio->CreateOutputStream(&callback, nullptr, info, 256, 1);

	//audio->StartStream(1, fLoopCallback, nullptr);


	auto aEnt = _builder->EntityC().Create();
	_builder->Audio()->BindEntity(aEnt);
	_builder->Audio()->AddAudio(aEnt, "Audio/BGMusic/mamb.wav", AudioMananger::AudioType::BG | AudioMananger::AudioType::Looping);
	_builder->Audio()->StartAudio(aEnt);
}

void APLState::Shutdown()
{
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
