#include "BasicWeapon.h"
#include "System.h"

BasicWeapon::BasicWeapon(EntityBuilder* builder, Entity player) : Weapon(builder)
{
	_timeSinceLastActivation = 100;
	_cooldown = 0.3f;
	_fire = false;
	_weaponEntity;
	_maxAmmo = -1;
	_currentAmmo = -1;
	_weaponEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_weaponEntity);
	_builder->Bounding()->CreateBoundingSphere(_weaponEntity, 0.05f);

	_builder->Light()->BindPointLight(_weaponEntity, XMFLOAT3(0, 0, 0), 0.1f, XMFLOAT3(0.0f, 0.0f, 1.0f), 5);

	_builder->Transform()->BindChild(player, _weaponEntity);

	Entity rot = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(rot);
	_builder->Light()->BindPointLight(rot, XMFLOAT3(0, 0, 0), 0.05f, XMFLOAT3(0.0f, 0.0f, 1.0f), 5);
	_builder->Light()->ChangeLightBlobRange(rot, 0.05f);
	_builder->Transform()->BindChild(_weaponEntity, rot);
	_builder->Transform()->SetPosition(rot, XMFLOAT3(0.06f, 0.0f, 0.0f));

	_active = true;
	//Entity e = _builder->EntityC().Create();
	//_builder->Event()->BindEventToEntity(e, EventManager::Type::Object);
	//_builder->Event()->BindEvent(e, EventManager::EventType::Update,
	//	[this]()
	//{
	//	
	//});
}

BasicWeapon::~BasicWeapon()
{
	
}

void BasicWeapon::Update(Entity playerEntity, float deltaTime)
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
		_projectiles.push_back(new BasicProjectile(playerEntity, _builder, 1.0f));
		_fire = false;
	}

}

void BasicWeapon::Shoot()
{
	if (System::GetInput()->IsMouseKeyDown(VK_LBUTTON))
		this->_Shoot();
}

bool BasicWeapon::HasAmmo()
{
	return true;
}

void BasicWeapon::_Shoot()
{
	if (_cooldown - _timeSinceLastActivation <= 0)
	{
		_fire = true;

		System::GetAudio()->PlaySoundEffect(L"basicattack.wav", 0.15f);

		_timeSinceLastActivation = 0.0;
	}
}