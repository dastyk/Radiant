#include "ShotgunWeapon.h"
#include "System.h"

ShotgunWeapon::ShotgunWeapon(EntityBuilder* builder, Entity player) : Weapon(builder, 4)
{
	_timeSinceLastActivation = 100;
	_cooldown = 0.8f;
	_maxAmmo = 10;
	_currentAmmo = 10;

	_builder->Light()->BindPointLight(_weaponEntity, XMFLOAT3(0, 0, 0), 0.1f, XMFLOAT3(0.0f, 1.0f, 0.0f), 5);
	_builder->Light()->ChangeLightBlobRange(_weaponEntity, 0.1f);
	_builder->Transform()->BindChild(player, _weaponEntity);


	if (System::GetOptions()->GetWeaponMode() == 0)
		_moveVector = XMFLOAT3(sqrtf(0.5f), 0.0f, -sqrtf(0.5f));
	else
		_moveVector = XMFLOAT3(0.12f, 0.125f, 0.0f);
}

ShotgunWeapon::~ShotgunWeapon()
{

}

void ShotgunWeapon::Update(const Entity& playerEntity, float deltaTime)
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

bool ShotgunWeapon::Shoot(const Entity& playerEntity)
{

	if (HasAmmo() && _cooldown - _timeSinceLastActivation <= 0 )
	{
		_currentAmmo -= 1;

		//_builder->Light()->ChangeLightBlobRange(_weaponEntity, 0.1f*(_currentAmmo/(float)_maxAmmo));
		_builder->Animation()->PlayAnimation(_weaponEntity, "scale", 0.1f*(_currentAmmo / (float)_maxAmmo) - _currentSize);
		System::GetInstance()->GetAudio()->PlaySoundEffect(L"basicattack.wav", 0.15f);

		_timeSinceLastActivation = 0.0;


		for (int i = 0; i < 15; i++)
			_projectiles.push_back(new ShotgunProjectile(playerEntity, _builder, 1.0f));
		return true;
	}
	return false;
}