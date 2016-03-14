#include "RegenPower.h"



RegenPower::RegenPower() : Power(nullptr)
{
}


RegenPower::~RegenPower()
{
}

void RegenPower::Update(Entity playerEntity, float deltaTime)
{
}

float RegenPower::Activate(bool & exec, float currentLight)
{
	return 0.0f;
}

bool RegenPower::Upgrade()
{
	return false;
}
