#include "ChoosePowerState.h"
#include "System.h"
#include "Shodan.h"

ChoosePowerState::ChoosePowerState()
{
	
	
}


ChoosePowerState::~ChoosePowerState()
{

}

void ChoosePowerState::Init()
{
	std::vector<FreePositions> dummyVar;
	List<EnemyWithStates> dummyList;

	Entity dummyEnt = _builder->EntityC().Create();
	_allPowers.push_back(new LockOnStrike(_builder, dummyEnt, &dummyList));
	_allPowers.push_back(new RandomBlink(_builder, dummyEnt, dummyVar));
	_allPowers.push_back(new CharmPower(_builder, dummyEnt, &dummyList));
	_allPowers.push_back(new TimeStopper(_builder, dummyEnt, &dummyList));
	_allPowers.push_back(new RegenPower(_builder, nullptr, dummyEnt));
	_allPowers.push_back(new LifeDrain(_builder, dummyEnt, &dummyList, nullptr));

	size_t firstPower = rand() % _allPowers.size();
	size_t secondPower = (firstPower + 1) % _allPowers.size();

	auto options = System::GetOptions();
	float pctX = options->GetScreenResolutionWidth() / 100.0f;
	float pctY = options->GetScreenResolutionHeight() / 100.0f;
	float midX = options->GetScreenResolutionWidth() / 2.0f;
	float midY = options->GetScreenResolutionHeight() / 2.0f;


	_choice2 = _builder->CreateOverlay(XMFLOAT3(midX - 30.0f * pctX, midY - 20.0f * pctY, 0.0f), 28.0f * pctX, 40.0f * pctY, "Assets/Textures/menuthing.png");
	_choice1 = _builder->CreateOverlay(XMFLOAT3(midX + 2.0f * pctX, midY - 20.0f * pctY, 0.0f), 28.0f * pctX, 40.0f * pctY, "Assets/Textures/menuthing.png");
	_choice2Text = _builder->CreateLabel(XMFLOAT3(midX + 4.0f * pctX, midY - 18.0f * pctY, 0.0f), _allPowers[firstPower]->GetDescription(40),40, XMFLOAT4(0.8f, 0.8f, 0.5f, 1.0f), 1.0f, 1.0f, "");
	_choice1Text = _builder->CreateLabel(XMFLOAT3(midX - 29.0f * pctX, midY - 18.0f * pctY, 0.0f), _allPowers[secondPower]->GetDescription(40), 40, XMFLOAT4(0.8f, 0.8f, 0.5f, 1.0f), 1.0f, 1.0f, "");



	_powerLabel = _builder->CreateLabel(XMFLOAT3(midX - 30.0f * pctX, midY - 10.0f * pctY - 20.0f * pctY, 0.0f), "Choose your powers", 40, XMFLOAT4(0.8f, 0.8f, 0.4f, 1.0f), 60.0f * pctX, 8.0f * pctY, "");

	_controller->Text()->ChangeFontSize(_powerLabel, 35);
	_controller->Text()->ChangeFontSize(_choice1Text, 20);
	_controller->Text()->ChangeFontSize(_choice2Text, 20);
	GameState* gstate = (GameState*)this->_savedState;
	auto i = System::GetInput();
	i->HideCursor(false);
	i->LockMouseToCenter(false);
	_builder->Event()->BindEvent(_choice1, EventManager::EventType::LeftClick, [this, i, firstPower, gstate]() {
		_powerToGive = _allPowers[firstPower]->GetType();
		i->LockMouseToCenter(true);
		_controller->ReleaseEntity(_choice1);
		_controller->ReleaseEntity(_choice2);
		_controller->ReleaseEntity(_powerLabel);
		_controller->ReleaseEntity(_choice1Text);
		_controller->ReleaseEntity(_choice2Text);
		i->HideCursor(true);
		gstate->ProgressNoNextLevel(_powerToGive);
		System::GetAudio()->PlaySoundEffect(L"choosepower.wav", 1.0f);
		ChangeStateTo(StateChange(gstate, false, true, false));

	});
	_builder->Event()->BindEvent(_choice2, EventManager::EventType::LeftClick, [this, i, secondPower, gstate]() {
		_powerToGive = _allPowers[secondPower]->GetType();
		i->LockMouseToCenter(true);
		i->LockMouseToWindow(true);
		_controller->ReleaseEntity(_choice1);
		_controller->ReleaseEntity(_choice2);
		_controller->ReleaseEntity(_powerLabel);
		_controller->ReleaseEntity(_choice1Text);
		_controller->ReleaseEntity(_choice2Text);
		i->HideCursor(true);
		gstate->ProgressNoNextLevel(_powerToGive);
		System::GetAudio()->PlaySoundEffect(L"choosepower.wav", 1.0f);
		ChangeStateTo(StateChange(gstate, false, true, false));
	});


	_camera = _builder->CreateCamera(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
	_builder->Transform()->SetDirection(_camera, XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
	_backgroundObject = _builder->CreateObject(XMVectorSet(0.0f, 0.0f, 3.0f, 1.0f),
		XMVectorSet(0, 0, 0, 0),
		XMVectorSet(32.0f, 32.0f, 1.0f, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/Dungeon/0/Wall_Dif.png",
		"Assets/Textures/Dungeon/0/Wall_NM.png",
		"Assets/Textures/Dungeon/0/Wall_Disp.png",
		"Assets/Textures/Dungeon/0/Wall_Roughness.png",
		"Assets/Textures/Dungeon/0/Wall_Glossiness.png");
	_builder->Material()->SetMaterialProperty(_backgroundObject, "ParallaxScaling", 0.04f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_backgroundObject, "ParallaxBias", -0.03f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_backgroundObject, "TexCoordScaleU", 32.0f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_backgroundObject, "TexCoordScaleV", 32.0f, "Shaders/GBuffer.hlsl");
	_light = _builder->EntityC().Create();
	_builder->Light()->BindPointLight(_light, XMFLOAT3(3.0f, 3.0f, 1.0f), 10.0f, XMFLOAT3(1.0f, 1.0f, 1.0f), 4.0f);

}

void ChoosePowerState::Shutdown()
{
	for (auto& i : _allPowers)
	{
		delete i;
	}
}

void ChoosePowerState::Update()
{
	State::Update();
}

void ChoosePowerState::Render()
{
	System::GetGraphics()->Render(_gameTimer.TotalTime(), _gameTimer.DeltaTime());
}
