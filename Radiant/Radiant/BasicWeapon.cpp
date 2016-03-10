#include "BasicWeapon.h"
#include "System.h"

BasicWeapon::BasicWeapon(EntityBuilder* builder, Entity player) : Weapon(builder,0)
{
	_timeSinceLastActivation = 100;
	_cooldown = 0.3f;
	_weaponEntity;
	_maxAmmo = -1;
	_currentAmmo = -1;

	_builder->Bounding()->CreateBoundingSphere(_weaponEntity, 0.05f);

	_builder->Light()->BindPointLight(_weaponEntity, XMFLOAT3(0, 0, 0), 0.1f, XMFLOAT3(0.0f, 0.0f, 1.0f), 5);

	_builder->Transform()->BindChild(player, _weaponEntity);

	_moveVector = XMFLOAT3(1.0f, 0.0f, 0.0f);

}

BasicWeapon::~BasicWeapon()
{
	
}

void BasicWeapon::Update(const Entity& playerEntity, float deltaTime)
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

bool BasicWeapon::Shoot(const Entity& playerEntity)
{
	if (_cooldown - _timeSinceLastActivation <= 0)
	{
		System::GetAudio()->PlaySoundEffect(L"basicattack.wav", 0.15f);

		_timeSinceLastActivation = 0.0;
		_projectiles.push_back(new BasicProjectile(playerEntity, _builder, 1.0f));
		return true;
	}
	return false;
}

bool BasicWeapon::HasAmmo()
{
	return true;
}
