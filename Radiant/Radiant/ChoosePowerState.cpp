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

	_controller->Text()->ChangeFontSize(_powerLabel, 20);
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
