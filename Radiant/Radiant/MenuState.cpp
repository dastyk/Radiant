#include "MenuState.h"
#include "System.h"


MenuState::MenuState() : State()
{
}


MenuState::~MenuState()
{
}

void MenuState::Init()
{
		auto i = System::GetInput();
	auto o = System::GetOptions();
	float width = (float)o->GetScreenResolutionWidth();
	float height = (float)o->GetScreenResolutionHeight();
	auto c = _controller;
	auto a = System::GetInstance()->GetAudio();
	float widthPercentOfDefault = (1.0f / 1920.0f) * width;
	float heightPercentOfDefault = (1.0f / 1080.0f) * height;
	float fontSize = 40 * widthPercentOfDefault;

	XMFLOAT4 TextColor = XMFLOAT4(41.0f / 255.0f, 127.0f / 255.0f, 185.0f / 255.0f, 1.0f);

	a->PlayBGMusic(L"mamb.wav", 0.5f);
	//Entity bar = _builder->CreateProgressBar(
	//	XMFLOAT3(200.0f, 200.0f, 0.0f),
	//	"Test:",
	//	150.0f,
	//	TextColor,
	//	0.0f,
	//	100.0f,
	//	50.0f,
	//	200.0f,
	//	50.0f);


	//_controller->BindEventHandler(bar, EventManager::Type::Overlay);
	//_controller->BindEvent(bar, EventManager::EventType::LeftClick,
	//	[this, bar]() 
	//{
	//	_controller->SetProgressBarValue(bar, 100.0f);
	//});

	//_controller->SetProgressBarValue(bar, 100.0f);
	// Background image
	/*_builder->CreateImage(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		width,
		height,
		"Assets/Textures/conceptArt.png");*/


	Entity wall = _builder->CreateObjectWithEmissive(
		XMVectorSet(0.0f, 1.0f, 2.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(100.0f, 3.0f, 1.0, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/Dungeon/0/Wall_Dif.png",
		"Assets/Textures/Dungeon/0/Wall_NM.png",
		"Assets/Textures/Dungeon/0/Wall_Disp.png",
		"Assets/Textures/Dungeon/0/Wall_Roughness.png",
		"Assets/Textures/Dungeon/0/Wall_Glow.png",
		"Assets/Textures/Dungeon/0/Wall_Glossiness.png");

	_builder->Material()->SetMaterialProperty(wall, "TexCoordScaleU", 100.0f, "Shaders/GBufferEmissive.hlsl");
	_builder->Material()->SetMaterialProperty(wall, "TexCoordScaleV", 3.0f, "Shaders/GBufferEmissive.hlsl");
	_builder->Material()->SetMaterialProperty(wall, "ParallaxBias", -0.01f, "Shaders/GBufferEmissive.hlsl");
	_builder->Material()->SetMaterialProperty(wall, "ParallaxScaling", 0.02f, "Shaders/GBufferEmissive.hlsl");
	_builder->Material()->SetMaterialProperty(wall, "EmissiveIntensity", 1.0f, "Shaders/GBufferEmissive.hlsl");
	_builder->Material()->SetMaterialProperty(wall, "EmissiveColor", XMFLOAT3(0.0f, 0.0f, 1.0f), "Shaders/GBufferEmissive.hlsl");

	_builder->Animation()->CreateAnimation(wall, "colordown", 5.0f, 
		[this, wall](float delta, float amount, float offset) 
	{
		XMFLOAT3 color = XMFLOAT3(1.0f*(1.0f - (offset + amount)), 0.0f, 1.0f*(offset + amount));
		_builder->Material()->SetMaterialProperty(wall, "EmissiveColor", color, "Shaders/GBufferEmissive.hlsl");

	}, 
		[this, wall]() 
	{
		_builder->Animation()->PlayAnimation(wall, "colorup", 1.0f, 0.0f);
	});

	_builder->Animation()->CreateAnimation(wall, "colorup", 5.0f,
		[this, wall](float delta, float amount, float offset)
	{
		XMFLOAT3 color = XMFLOAT3(1.0f*(1.0f - amount), 0.0f, 1.0f*(amount));
		_builder->Material()->SetMaterialProperty(wall, "EmissiveColor", color, "Shaders/GBufferEmissive.hlsl");

	},
		[this, wall]()
	{
		_builder->Animation()->PlayAnimation(wall, "colordown", -1.0f, 1.0f);
	});

	_builder->Animation()->PlayAnimation(wall, "colordown", -1.0f, 1.0f);


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


	Entity li = _builder->EntityC().Create();
	_builder->Light()->BindPointLight(li, XMFLOAT3(1.5f, 1.0f, 1.0f), 3.0f, XMFLOAT3(1.0f, 1.0f, 1.0f), 0.5f);
	_builder->Light()->SetAsVolumetric(li, false);
	
	Entity li2 = _builder->EntityC().Create();
	_builder->Light()->BindPointLight(li2, XMFLOAT3(-2.0f, 0.5f, -0.0f), 4.0f, XMFLOAT3(0.2f, 0.5f, 0.8f), 0.3f);
	_builder->Light()->ChangeLightBlobRange(li2, 1.5f);

	Entity li3 = _builder->EntityC().Create();
	_builder->Light()->BindPointLight(li3, XMFLOAT3(2.0f, 0.5f, 1.25f), 2.5f, XMFLOAT3(0.2f, 0.8f, 0.4f), 0.4f);
	_builder->Light()->ChangeLightBlobRange(li3, 0.5f);
	_builder->Transform()->CreateTransform(li3);
	_builder->Transform()->SetPosition(li3, XMFLOAT3(2.0f, 0.1f, 0.0f));
	_builder->Transform()->SetRotation(li3, XMFLOAT3(0.0f, -90.0f, 0.0f));
	_controller->BindEventHandler(li3, EventManager::Type::Object);
	_controller->BindEvent(li3, EventManager::EventType::Update,
		[this,li3]() 
	{
		_controller->Transform()->RotateYaw(li3, -45.0f*_gameTimer.DeltaTime());
		_controller->Transform()->MoveForward(li3, 0.5f*_gameTimer.DeltaTime());
	
	});

	Entity cam = _builder->CreateCamera(XMVectorSet(0.0f, 0.5f, -2.0f, 0.0f));


	Entity per = _builder->EntityC().Create();
	_builder->Mesh()->CreateStaticMesh(per, "Assets/Models/cube.arf");
	_builder->Material()->BindMaterial(per, "Shaders/Emissive.hlsl");


	_builder->Transform()->CreateTransform(per);

	_builder->Material()->SetEntityTexture(per, "DiffuseMap", L"Assets/Textures/per.png");

	_builder->Transform()->SetPosition(per, XMFLOAT3(0.5f, -1.8f, 0.7f));
	_builder->Transform()->SetScale(per, XMFLOAT3(1.0f, 1.0f, 0.01f));

	_controller->Transform()->BindChild(cam, per);


	_builder->Transform()->SetRotation(cam, XMFLOAT3(15.0f, 0.0f, 0.0f));


	_builder->Animation()->CreateAnimation(per, "wait", 240.0f,
		[](float delta, float amount, float offset)
	{},
		[this, per]()
	{

		_controller->Animation()->PlayAnimation(per, "moveup", 1.35f);
	});


	_builder->Animation()->CreateAnimation(per, "moveup",5.0f,
		[this, per](float delta, float amount, float offset)
		{
		_controller->Transform()->MoveUp(per, delta);
	},
		[this, per,a]()
		{
		_controller->Animation()->PlayAnimation(per, "wait2", 0.04f);
		a->PlaySoundEffect(L"pew.wav", 1);
	});

	_builder->Animation()->CreateAnimation(per, "wait2", 1.0f,
		[](float delta, float amount, float offset)
	{},
		[this, per]()
		{

		_controller->Animation()->PlayAnimation(per, "movedown", 1.35f);
	});


	_builder->Animation()->CreateAnimation(per, "movedown", 5.0f,
		[this, per](float delta, float amount, float offset)
			{
		_controller->Transform()->MoveDown(per,  delta);
	},
		[this, per]()
			{
		_controller->Animation()->PlayAnimation(per, "wait", 0.04f);
			
	});

	_controller->Animation()->PlayAnimation(per, "wait", 0.04f);


	_builder->CreateHealingLight(XMFLOAT3(-1.0f, 2.5f, 1.0f), XMFLOAT3(120.0f, -20.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 5.0f, XMConvertToRadians(30.0f), XMConvertToRadians(20.0f),10.0f);

	// Radiant text
	Entity rtext = _builder->CreateLabel(
		XMFLOAT3(width / 2.0f - 100.0f, 25.0f, 0.0f),
		"Radiant",
		fontSize,
		TextColor,
		250.0f,
		45.0f,
		"");

	_builder->Transform()->SetPosition(rtext, XMFLOAT3(width / 2.0f - _builder->Text()->GetLength(rtext) / 2.0f, 25.0f, 0.0f));



	Entity li5 = _builder->EntityC().Create();
	_builder->Light()->BindPointLight(li5, XMFLOAT3(-2.0f, 0.5f, -0.0f), 4.0f, XMFLOAT3(0.2f, 0.5f, 0.8f), 0.3f);
	_builder->Light()->ChangeLightBlobRange(li5, 0.5f);
	_builder->Transform()->CreateTransform(li5);
	_builder->Transform()->SetPosition(li5, XMFLOAT3(0.0f, 0.5f, 1.0f));

	_builder->Animation()->CreateAnimation(li5, "scaleu", 2.0f, 
		[this,li5](float delta, float amount, float offset)
	{
		_builder->Light()->ChangeLightBlobRange(li5, 0.5f + amount);
	},
		[this, li5]()
	{
		_builder->Animation()->PlayAnimation(li5, "scaled", 0.5f);
	});

	_builder->Animation()->CreateAnimation(li5, "scaled", 2.0f,
		[this, li5](float delta, float amount, float offset)
	{
		_builder->Light()->ChangeLightBlobRange(li5, 1.0f - amount);
	},
		[this, li5]()
	{
		_builder->Animation()->PlayAnimation(li5, "scaleu", 0.5f);
	});


	_builder->Animation()->PlayAnimation(li5, "scaleu", 0.5f);





	// Start game button
	Entity b1 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 230.0f, 0.0f),
		"Start Game",
		fontSize,
		TextColor,
		"",
		[i, a]()
	{
		a->PlaySoundEffect(L"menuclick.wav", 1);
		ChangeStateTo(StateChange(new GameState()));
	});

	// Controlls button
	Entity b6 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 180.0f, 0.0f),
		"Controls",
		fontSize,
		TextColor,
		"",
		[i, a]()
	{
		a->PlaySoundEffect(L"menuclick.wav", 1);
		ChangeStateTo(StateChange(new ControllsState));
	});

	//Options button
	Entity b5 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 130.0f, 0.0f),
		"Options",
		fontSize,
		TextColor,
		"",
		[i, a]()
	{
		a->PlaySoundEffect(L"menuclick.wav", 1);
		ChangeStateTo(StateChange(new OptionsState()));
	});


	// Exit button
	Entity b2 = _builder->CreateButton(
		XMFLOAT3(50.0f, height - 80.0f, 0.0f),
		"Exit",
		fontSize,
		TextColor,
		"",
		[a]() {
		a->PlaySoundEffect(L"menuclick.wav", 1);
		ExitApplication;
	});

	_controller->BindEvent(b2, EventManager::EventType::Update, [i]()
	{
		if (i->IsKeyPushed(VK_ESCAPE))
		{
			ExitApplication;
		}
		if (i->IsKeyPushed('1'))
		{
			ChangeStateTo(StateChange(new GameState()));
		}
		if (i->IsKeyPushed('2'))
		{
			ChangeStateTo(StateChange(new ControllsState()));
		}
		if (i->IsKeyPushed('3'))
		{
			ChangeStateTo(StateChange(new OptionsState()));
		}
	}
	);
}

void MenuState::Shutdown()
{
}

