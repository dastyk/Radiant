#include "RandomBlink.h"
#include "System.h"

RandomBlink::RandomBlink(EntityBuilder* builder, Entity player, vector<FreePositions> positions) : Power(builder)
{
	_timeSinceLastActivation = 100;
	_cooldown = 5.0;
	_lightIntensity = 0.0f;
	_powerLevel = 0;

	_powerEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_powerEntity);
	_builder->Bounding()->CreateBoundingSphere(_powerEntity, 0.05f);

	_builder->Light()->BindPointLight(_powerEntity, XMFLOAT3(0, 0, 0), 8.0f, XMFLOAT3(1.0f, 1.0f, 1.0f), _lightIntensity);

	_builder->Transform()->BindChild(player, _powerEntity);

	viablePositions = positions;
	_loading = false;
}

RandomBlink::~RandomBlink()
{
	_builder->GetEntityController()->ReleaseEntity(_powerEntity);
}

void RandomBlink::Update(Entity playerEntity, float deltaTime)
{
	_timeSinceLastActivation += deltaTime;

	if (_loading)
	{
		_lightIntensity += deltaTime * 80;
		_builder->Light()->ChangeLightIntensity(_powerEntity, _lightIntensity);
	}
	else
	{
		_lightIntensity -= deltaTime * 80;

		if (_lightIntensity < 0.0f)
			_lightIntensity = 0.0f;

		_builder->Light()->ChangeLightIntensity(_powerEntity, _lightIntensity);
	}

	if (_loading && _lightIntensity >= MAXLIGHTINTENSITY)
	{
		_loading = false;

		int randValue = rand() % viablePositions.size();
		XMFLOAT3 temp;
		XMStoreFloat3(&temp, _builder->Transform()->GetPosition(playerEntity));
		temp.x = (float)viablePositions[randValue].x;
		temp.z = (float)viablePositions[randValue].y;
		_builder->Transform()->SetPosition(playerEntity, temp);

		System::GetAudio()->PlaySoundEffect(L"teleport.wav", 0.5f);
	}


}

float RandomBlink::Activate(bool& exec, float currentLight)
{
	if (_cooldown - _timeSinceLastActivation <= 0.0f && currentLight >= 2.0f)
	{
		_timeSinceLastActivation = 0.0f;
		_loading = true;
		exec = true;
		return 2.0f;
	}
	exec = false;
	return 0.0f;
}

bool RandomBlink::Upgrade()
{
	if (_powerLevel <= 5)
	{
		_cooldown -= 0.5f;
		_powerLevel++;
		return true;
	}

	return false;
}