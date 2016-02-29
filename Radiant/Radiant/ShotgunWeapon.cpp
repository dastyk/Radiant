#include "ShotgunWeapon.h"
#include "System.h"

ShotgunWeapon::ShotgunWeapon(EntityBuilder* builder, Entity player) : Weapon(builder)
{
	_timeSinceLastActivation = 100;
	_cooldown = 0.8f;
	_fire = false;
	_maxAmmo = 10;
	_currentAmmo = 10;
	_weaponEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_weaponEntity);

	_builder->Light()->BindPointLight(_weaponEntity, XMFLOAT3(0, 0, 0), 0.1f, XMFLOAT3(0.0f, 1.0f, 0.0f), 5);
	_builder->Light()->ChangeLightBlobRange(_weaponEntity, 0.1f);
	_builder->Transform()->BindChild(player, _weaponEntity);

	Entity rot = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(rot);
	_builder->Light()->BindPointLight(rot, XMFLOAT3(0, 0, 0), 0.05f, XMFLOAT3(0.0f, 1.0f, 0.0f), 5);
	_builder->Light()->ChangeLightBlobRange(rot, 0.05f);
	_builder->Transform()->BindChild(_weaponEntity, rot);
	_builder->Transform()->MoveRight(rot, 0.06f);

	_builder->GetEntityController()->BindEventHandler(_weaponEntity, EventManager::Type::Object);
	_builder->GetEntityController()->BindEvent(_weaponEntity, EventManager::EventType::Update,
		[this]() 
	{
		_builder->Transform()->RotateYaw(_weaponEntity, 2);
	});

	_builder->Transform()->MoveForward(_weaponEntity, 0.2f);
	_builder->Transform()->MoveRight(_weaponEntity, 0.07f);
	_builder->Transform()->MoveDown(_weaponEntity, 0.05f);

	_active = true;

	//Entity e = _builder->EntityC().Create();
	//_builder->Event()->BindEventToEntity(e, EventManager::Type::Object);
	//_builder->Event()->BindEvent(e, EventManager::EventType::Update,
	//	[this]()
	//{
	//	
	//});
}

ShotgunWeapon::~ShotgunWeapon()
{

}

void ShotgunWeapon::Update(Entity playerEntity, float deltaTime)
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
		for (int i = 0; i < 15; i++)
			_projectiles.push_back(new ShotgunProjectile(playerEntity, _builder));

		_fire = false;
	}

}

void ShotgunWeapon::Shoot()
{
	if (System::GetInput()->IsMouseKeyDown(VK_LBUTTON) && HasAmmo())
		this->_Shoot();
}

void ShotgunWeapon::_Shoot()
{
	if (_cooldown - _timeSinceLastActivation <= 0)
	{
		_currentAmmo -= 1;
		_builder->Light()->ChangeLightBlobRange(_weaponEntity, 0.1f*(_currentAmmo/(float)_maxAmmo));
		_fire = true;

		System::GetInstance()->GetAudio()->PlaySoundEffect(L"basicattack.wav", 0.15f);

		_timeSinceLastActivation = 0.0;
	}
}