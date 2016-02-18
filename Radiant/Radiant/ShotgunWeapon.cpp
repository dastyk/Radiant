#include "ShotgunWeapon.h"
#include "System.h"

ShotgunWeapon::ShotgunWeapon(EntityBuilder* builder) : Weapon(builder)
{
	_timeSinceLastActivation = 100;
	_cooldown = 0.8f;
	_fire = false;

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