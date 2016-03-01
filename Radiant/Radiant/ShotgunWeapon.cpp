#include "ShotgunWeapon.h"
#include "System.h"

ShotgunWeapon::ShotgunWeapon(EntityBuilder* builder, Entity player) : Weapon(builder)
{
	_timeSinceLastActivation = 100;
	_cooldown = 0.8f;
	_fire = false;

	_weaponEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_weaponEntity);
	_builder->Bounding()->CreateBoundingSphere(_weaponEntity, 0.05f);

	_builder->Light()->BindPointLight(_weaponEntity, XMFLOAT3(0, 0, 0), 0.1f, XMFLOAT3(0.0f, 1.0f, 0.0f), 5);

	_builder->Transform()->BindChild(player, _weaponEntity);
	_builder->Transform()->MoveForward(_weaponEntity, 0.2f);
	_builder->Transform()->MoveRight(_weaponEntity, 0.15f);
	_builder->Transform()->MoveDown(_weaponEntity, 0.1f);

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
			_projectiles.push_back(new ShotgunProjectile(playerEntity, _builder, 1.0f));

		_fire = false;
	}

}

void ShotgunWeapon::Shoot()
{
	if (System::GetInput()->IsMouseKeyDown(VK_LBUTTON))
		this->_Shoot();
}

void ShotgunWeapon::_Shoot()
{
	if (_cooldown - _timeSinceLastActivation <= 0)
	{
		_fire = true;

		System::GetInstance()->GetAudio()->PlaySoundEffect(L"basicattack.wav", 0.15f);

		_timeSinceLastActivation = 0.0;
	}
}