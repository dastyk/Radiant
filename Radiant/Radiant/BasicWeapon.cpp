#include "BasicWeapon.h"
#include "System.h"

BasicWeapon::BasicWeapon(EntityBuilder* builder) : _builder(builder)
{
	_timeSinceLastActivation = 100;
	_cooldown = 0.2;
	_fire = false;

	Entity e = _builder->EntityC().Create();
	_builder->Event()->BindEventToEntity(e, EventManager::Type::Object);
	_builder->Event()->BindEvent(e, EventManager::EventType::LeftClick,
		[this]()
	{
		this->_Shoot();
	});
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
		_projectiles.push_back(new BasicProjectile(playerEntity, _builder));
		_fire = false;
	}

}

void BasicWeapon::_Shoot()
{
	if (_cooldown - _timeSinceLastActivation <= 0)
	{
		_fire = true;

		System::GetAudio()->PlaySoundEffect(L"basicattack.wav", 1);

		_timeSinceLastActivation = 0.0;
	}
}