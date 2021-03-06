#include "ChargeWeapon.h"
#include "System.h"

ChargeWeapon::ChargeWeapon(EntityBuilder* builder, Entity weppos, Entity player) : Weapon(builder, 0)
{
	_timeSinceLastActivation = 100;
	_cooldown = 1.0f;
	_fire = false;
	_weaponEntity;
	_maxAmmo = 1;
	_currentAmmo = 1;
	_chargeTime = 0.0f;
	_chargedLastFrame = false;

	_builder->Bounding()->CreateBoundingSphere(_weaponEntity, 0.05f);
	_builder->Light()->BindPointLight(_weaponEntity, XMFLOAT3(0, 0, 0), 0.1f, XMFLOAT3(0.0f, 0.5f, 0.5f), 5);
	_builder->Transform()->BindChild(weppos, _weaponEntity);

	
	_chargeEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_chargeEntity);
	_builder->Bounding()->CreateBoundingSphere(_chargeEntity, 0.05f);
	_builder->Light()->BindPointLight(_chargeEntity, XMFLOAT3(0, 0, 0), 0.0f, XMFLOAT3(0.0f, 0.5f, 0.5f), 0.0f);
	_builder->Light()->ChangeLightBlobRange(_chargeEntity, 0.0f);
	_builder->Transform()->SetPosition(_chargeEntity, XMFLOAT3(0.0f, 0.0f, 2.1f));
	_builder->Transform()->BindChild(player, _chargeEntity);


	if (System::GetOptions()->GetWeaponMode() == 0)
		_moveVector = XMFLOAT3(0.0f, 0.0f, 0.5f);
	else
		_moveVector = XMFLOAT3(0.12f, -0.0f, 0.0f);
}

ChargeWeapon::~ChargeWeapon()
{
	_builder->GetEntityController()->ReleaseEntity(_chargeEntity);
}

void ChargeWeapon::Update(const Entity& playerEntity, float deltaTime)
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

	if (_fire)
	{
		_chargeTime += deltaTime;

		if (_chargeTime > 2.0f)
		{
			_chargeTime = 2.0f;
		}

	/*	_builder->Transform()->SetPosition(_chargeEntity, _builder->Transform()->GetPosition(playerEntity));
		_builder->Transform()->SetRotation(_chargeEntity, _builder->Transform()->GetRotation(playerEntity));
		_builder->Transform()->MoveForward(_chargeEntity, 2.1f);*/

		_builder->Light()->ChangeLightIntensity(_chargeEntity, _chargeTime * 20);
		_builder->Light()->ChangeLightBlobRange(_chargeEntity, _chargeTime);
		_builder->Light()->ChangeLightRange(_chargeEntity, _chargeTime * 4);

		_fire = false;
	}
	else if (_chargedLastFrame)
	{
		_projectiles.push_back(new ChargeProjectile(playerEntity, _builder, 1.0f, _chargeTime));
		_timeSinceLastActivation = 0.0f;
		_fire = false;
		_chargedLastFrame = false;
		_chargeTime = 0.0f;

		System::GetAudio()->PlaySoundEffect(L"basicattack.wav", min(_chargeTime * 0.4f / 3.0f, 0.4f));

		_builder->Light()->ChangeLightIntensity(_chargeEntity, 0.0f);
		_builder->Light()->ChangeLightBlobRange(_chargeEntity, 0.0f);
		_builder->Light()->ChangeLightRange(_chargeEntity, 0.0f);
	}

}

bool ChargeWeapon::Shoot(const Entity& playerEntity)
{
	if (!_chargedLastFrame && _cooldown - _timeSinceLastActivation <= 0)
	{
		_fire = true;
		_chargedLastFrame = true;
		return true;
	}
	else
	{
		_fire = true;
		_chargedLastFrame = true;
		return false;
	}
	return false;
}

bool ChargeWeapon::HasAmmo()
{
	return true;
}