#include "RegenPower.h"



RegenPower::RegenPower(EntityBuilder* builder, Player* thePlayer, Entity playerEnt) : Power(builder)
{
	_player = thePlayer;

	_timeSinceLastActivation = 100;
	_cooldown = 20.0f;
	_status = -1;
	_powerLevel = 0;
	_regenPercent = 0.4f;
	_type = power_id_t::REGENPOWER;

	_powerEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_powerEntity);
	_builder->Bounding()->CreateBoundingSphere(_powerEntity, 0.05f);
	_builder->Light()->BindPointLight(_powerEntity, XMFLOAT3(0, 0, 0), 0.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), 20.0f);
	_builder->Transform()->BindChild(playerEnt, _powerEntity);

	_powerName = "Regenerate Light";
	_description = "Quickly regenerate the lightpool that your powers use";

	_decalTexture = "Assets/Textures/PowerRegen.png";
}

RegenPower::~RegenPower()
{
}

void RegenPower::Update(Entity playerEntity, float deltaTime)
{
	if (_status == 1)
	{
		_regenTime += deltaTime;
		_regenTime = fmod(_regenTime, 1.0f);
		_builder->Light()->ChangeLightRange(_powerEntity, pow(4, _regenTime));
}
	else
	{
		_regenTime = 0.0f;
	}
}

float RegenPower::Activate(bool & exec, float currentLight)
{
	_status = _status * (-1);

	if (_status == 1)
	{
		_player->RegenerateLight(_regenPercent);
	}
	else
	{
		_player->ResetRegen();
	}

	exec = false;
	return 0.0f;
}

bool RegenPower::Upgrade()
{
	//increase _regenPercent here or something

	return false;
}
