#include "FragBombWeapon.h"
#include "System.h"

FragBombWeapon::FragBombWeapon(EntityBuilder* builder, Entity player) : Weapon(builder)
{
	_timeSinceLastActivation = 100;
	_cooldown = 2.0f;
	_fire = false;
	_maxAmmo = 3;
	_currentAmmo = 3;
	_weaponEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_weaponEntity);
	_builder->Bounding()->CreateBoundingSphere(_weaponEntity, 0.05f);

	_builder->Light()->BindPointLight(_weaponEntity, XMFLOAT3(0, 0, 0), 0.1f, XMFLOAT3(1.0f, 0.0f, 1.0f), 5);
	_builder->Light()->ChangeLightBlobRange(_weaponEntity, 0.1f);
	_builder->Transform()->BindChild(player, _weaponEntity);


	Entity rot = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(rot);
	_builder->Light()->BindPointLight(rot, XMFLOAT3(0, 0, 0), 0.05f, XMFLOAT3(1.0f, 0.0f, 1.0f), 5);
	_builder->Light()->ChangeLightBlobRange(rot, 0.05f);
	_builder->Transform()->BindChild(_weaponEntity, rot);
	_builder->Transform()->SetPosition(rot, XMFLOAT3(-0.06f, 0.0f, 0.0f));

	_active = true;

	//Entity e = _builder->EntityC().Create();
	//_builder->Event()->BindEventToEntity(e, EventManager::Type::Object);
	//_builder->Event()->BindEvent(e, EventManager::EventType::Update,
	//	[this]()
	//{
	//	
	//});
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

				for (int j = 0; j < 200; j++)
				{
					XMFLOAT3 temp;
					XMStoreFloat3(&temp, _builder->Transform()->GetPosition(_projectiles[i]->GetEntity())); // fix so split works

					_projectiles.push_back(new FragBombProjectile(temp, _builder, 1.0f));

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
		_projectiles.push_back(new FragBombProjectile(playerEntity, _builder, 1.0f));
		_fire = false;
		_currentAmmo -= 1;
		_builder->Light()->ChangeLightBlobRange(_weaponEntity, 0.1f*(_currentAmmo / (float)_maxAmmo));
		_children.push_back(false);

	}

}

void FragBombWeapon::Shoot()
{
	if (System::GetInput()->IsMouseKeyDown(VK_LBUTTON) && HasAmmo())
		this->_Shoot();
}

void FragBombWeapon::_Shoot()
{
	if (_cooldown - _timeSinceLastActivation <= 0)
	{
		_fire = true;

		System::GetAudio()->PlaySoundEffect(L"basicattack.wav", 0.15f);

		_timeSinceLastActivation = 0.0;
	}
}