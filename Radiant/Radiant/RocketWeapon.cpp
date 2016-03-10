#include "RocketWeapon.h"
#include "System.h"

RocketWeapon::RocketWeapon(EntityBuilder* builder, Entity player) : Weapon(builder, 3)
{
	_timeSinceLastActivation = 100;
	_cooldown = 1.0f;
	_maxAmmo = 10;
	_currentAmmo = 10;

	_builder->Bounding()->CreateBoundingSphere(_weaponEntity, 0.05f);

	_builder->Light()->BindPointLight(_weaponEntity, XMFLOAT3(0, 0, 0), 0.1f, XMFLOAT3(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f), 5);
	_builder->Light()->ChangeLightBlobRange(_weaponEntity, 0.1f);
	_builder->Transform()->BindChild(player, _weaponEntity);

	_moveVector = XMFLOAT3(-sqrtf(0.5f), 0.0f, -sqrtf(0.5f));
}

RocketWeapon::~RocketWeapon()
{

}

void RocketWeapon::Update(const Entity& playerEntity, float deltaTime)
{
	_timeSinceLastActivation += deltaTime;

	for (int i = 0; i < _projectiles.size(); i++)
	{
		_projectiles[i]->Update(deltaTime);
	}

	for (int i = 0; i < _projectiles.size(); i++)
	{
		if (!_projectiles[i]->GetState())
		{
			delete _projectiles[i];
			_projectiles.erase(_projectiles.begin() + i);
			_projectiles.shrink_to_fit();
			i--;
		}
	}


}

bool RocketWeapon::Shoot(const Entity& playerEntity)
{
	if (HasAmmo() && _cooldown - _timeSinceLastActivation <= 0)
	{
		_currentAmmo -= 1;
		//_builder->Light()->ChangeLightBlobRange(_weaponEntity, 0.1f*(_currentAmmo / (float)_maxAmmo));
		_builder->Animation()->PlayAnimation(_weaponEntity, "scale", 0.1f*(_currentAmmo / (float)_maxAmmo) - _currentSize);
		System::GetAudio()->PlaySoundEffect(L"basicattack.wav", 0.15f);

		_timeSinceLastActivation = 0.0;

		_projectiles.push_back(new RocketProjectile(playerEntity, _builder, 1.0f));
		return true;
	}
	return false;
}
