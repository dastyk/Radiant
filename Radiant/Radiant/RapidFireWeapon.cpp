#include "RapidFireWeapon.h"
#include "System.h"

RapidFireWeapon::RapidFireWeapon(EntityBuilder* builder, Entity player) : Weapon(builder,3)
{
	_timeSinceLastActivation = 100;
	_cooldown = 0.075f;
	_maxAmmo = 100;
	_currentAmmo = 100;

	_builder->Bounding()->CreateBoundingSphere(_weaponEntity, 0.05f);

	_builder->Light()->BindPointLight(_weaponEntity, XMFLOAT3(0, 0, 0), 0.1f, XMFLOAT3(1.0f, 0.0f, 0.0f), 5);
	_builder->Light()->ChangeLightBlobRange(_weaponEntity, 0.1f);
	_builder->Transform()->BindChild(player, _weaponEntity);

	
	if (System::GetOptions()->GetWeaponMode() == 0)
		_moveVector = XMFLOAT3(-sqrtf(0.5f), 0.0f, -sqrtf(0.5f));
	else
		_moveVector = XMFLOAT3(0.12f, 0.075f, 0.0f);
}

RapidFireWeapon::~RapidFireWeapon()
{

}

void RapidFireWeapon::Update(const Entity& playerEntity, float deltaTime)
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

bool RapidFireWeapon::Shoot(const Entity& playerEntity)
{
	if (HasAmmo() && _cooldown - _timeSinceLastActivation <= 0)
	{
		_currentAmmo -= 1;
		//_builder->Light()->ChangeLightBlobRange(_weaponEntity, 0.1f*(_currentAmmo / (float)_maxAmmo));
		_builder->Animation()->PlayAnimation(_weaponEntity, "scale", 0.1f*(_currentAmmo / (float)_maxAmmo) - _currentSize);
//		System::GetAudio()->PlaySoundEffect(L"basicattack.wav", 0.15f);

		_timeSinceLastActivation = 0.0;
		_projectiles.push_back(new RapidFireProjectile(playerEntity, _builder, 1.0f));
		return true;
	}
	return false;
}