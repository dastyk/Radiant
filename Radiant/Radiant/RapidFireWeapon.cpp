#include "RapidFireWeapon.h"
#include "System.h"

RapidFireWeapon::RapidFireWeapon(EntityBuilder* builder) : Weapon(builder)
{
	_timeSinceLastActivation = 100;
	_cooldown = 0.075;
	_fire = false;

	Entity e = _builder->EntityC().Create();
	_builder->Event()->BindEventToEntity(e, EventManager::Type::Object);
	_builder->Event()->BindEvent(e, EventManager::EventType::Update,
		[this]()
	{
		if (System::GetInput()->IsMouseKeyDown(VK_LBUTTON))
			this->_Shoot();
	});
}

RapidFireWeapon::~RapidFireWeapon()
{

}

void RapidFireWeapon::Update(Entity playerEntity, float deltaTime)
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
		_projectiles.push_back(new RapidFireProjectile(playerEntity, _builder));
		_fire = false;
	}

}

void RapidFireWeapon::_Shoot()
{
	if (_cooldown - _timeSinceLastActivation <= 0)
	{
		_fire = true;

		System::GetAudio()->PlaySoundEffect(L"basicattack.wav", 1);

		_timeSinceLastActivation = 0.0;
	}
}