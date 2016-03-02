#include "BounceWeapon.h"
#include "System.h"

BounceWeapon::BounceWeapon(EntityBuilder* builder, Entity player) : Weapon(builder,4)
{
	_timeSinceLastActivation = 100;
	_cooldown = 0.5;
	_fire = false;
	_weaponEntity;
	
	_maxAmmo = 30;
	_currentAmmo = 30;

	_builder->Bounding()->CreateBoundingSphere(_weaponEntity, 0.05f);
	
	_builder->Light()->BindPointLight(_weaponEntity, XMFLOAT3(0, 0, 0), 0.1f, XMFLOAT3(1.0f, 165.0f / 255.0f, 0.0f), 5);

	_builder->Light()->ChangeLightBlobRange(_weaponEntity, 0.1f);
	_builder->Transform()->BindChild(player, _weaponEntity);

	_moveVector = XMFLOAT3(sqrtf(0.5f),0.0f , sqrtf(0.5f));
	
	_active = true;
}

BounceWeapon::~BounceWeapon()
{

}

void BounceWeapon::Update(Entity playerEntity, float deltaTime)
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

	if (_fire == true)
	{
		_projectiles.push_back(new BounceProjectile(playerEntity, _builder));
		_fire = false;
	}

}

void BounceWeapon::Shoot()
{
	if (System::GetInput()->IsMouseKeyDown(VK_LBUTTON))
		this->_Shoot();
}

void BounceWeapon::_Shoot()
{
	if (_cooldown - _timeSinceLastActivation <= 0)
	{
		_fire = true;

		System::GetAudio()->PlaySoundEffect(L"basicattack.wav", 0.15f);
		_currentAmmo -= 1;
		_builder->Animation()->PlayAnimation(_weaponEntity, "scale", 0.1f*(_currentAmmo / (float)_maxAmmo) - _currentSize);
		_timeSinceLastActivation = 0.0;
	}
}