#include "FragBombWeapon.h"
#include "System.h"

FragBombWeapon::FragBombWeapon(EntityBuilder* builder) : Weapon(builder)
{
	_timeSinceLastActivation = 100;
	_cooldown = 2.0f;
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

FragBombWeapon::~FragBombWeapon()
{

}

void FragBombWeapon::Update(Entity playerEntity, float deltaTime)
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
			if (_children[i])
			{
				delete _projectiles[i];

				_projectiles.erase(_projectiles.begin() + i);
				_projectiles.shrink_to_fit();

				_children.erase(_children.begin() + i);
				_children.shrink_to_fit();

				i--;
			}
			else
			{
				for (int j = 0; j < 30; j++)
				{
					XMFLOAT3 temp;
					XMStoreFloat3(&temp, _builder->Transform()->GetPosition(_projectiles[i]->GetEntity())); // fix so split works

					_projectiles.push_back(new FragBombProjectile(temp, _builder));

					_children.push_back(true);



				}

				delete _projectiles[i];

				_projectiles.erase(_projectiles.begin() + i);
				_projectiles.shrink_to_fit();

				_children.erase(_children.begin() + i);
				_children.shrink_to_fit();
			}
		}
	}

	if (_fire == true)
	{
		_projectiles.push_back(new FragBombProjectile(playerEntity, _builder));
		_fire = false;
		_children.push_back(false);

	}

}

void FragBombWeapon::_Shoot()
{
	if (_cooldown - _timeSinceLastActivation <= 0)
	{
		_fire = true;

		System::GetAudio()->PlaySoundEffect(L"basicattack.wav", 1);

		_timeSinceLastActivation = 0.0;
	}
}