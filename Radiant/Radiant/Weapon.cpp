#include "Weapon.h"
#include "System.h"

Weapon::Weapon(EntityBuilder* builder, unsigned int type) : _builder(builder),_type(type),_cooldown(0.0f),_timeSinceLastActivation(0.0f), _maxAmmo(0), _currentAmmo(0), _active(true), _uneS(0.2f), _eS(0.1f)
{
	_weaponEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_weaponEntity);

	if (System::GetOptions()->GetWeaponMode() == 0)
	{

		_builder->Animation()->CreateAnimation(_weaponEntity, "moveout", 0.25f,
			[this](float delta, float amount, float offset)
		{
			_builder->Transform()->MoveAlongVector(_weaponEntity, XMLoadFloat3(&_moveVector)*delta*0.06f);
		});

		_builder->Animation()->CreateAnimation(_weaponEntity, "movein", 0.25f,
			[this](float delta, float amount, float offset)
		{
			_builder->Transform()->MoveAlongVector(_weaponEntity, XMLoadFloat3(&_moveVector)*delta*-0.06f);
		});


		_builder->Animation()->CreateAnimation(_weaponEntity, "scaleup", 0.25f,
			[this](float delta, float amount, float offset)
		{
			_currentSize += delta;
			_builder->Light()->ChangeLightBlobRange(_weaponEntity, _currentSize);
		});


		_builder->Animation()->CreateAnimation(_weaponEntity, "scaledown", 0.25f,
			[this](float delta, float amount, float offset)
		{
			_currentSize -= delta;
			_builder->Light()->ChangeLightBlobRange(_weaponEntity, _currentSize);
		});

		_builder->Animation()->CreateAnimation(_weaponEntity, "scale", 0.25f,
			[this](float delta, float amount, float offset)
		{
			_currentSize += delta;
			_builder->Light()->ChangeLightBlobRange(_weaponEntity, _currentSize);
		});


		_moveVector = XMFLOAT3(1.0f, 0.0f, 0.0f);
		_currentSize = _eS;
	}
	else
	{
		_builder->Animation()->CreateAnimation(_weaponEntity, "moveout", 0.25f,
			[this](float delta, float amount, float offset)
		{
			_builder->Transform()->SetPosition(_weaponEntity, XMLoadFloat3(&_moveVector)*amount);
		});

		_builder->Animation()->CreateAnimation(_weaponEntity, "movein", 0.25f,
			[this](float delta, float amount, float offset)
		{
			_builder->Transform()->SetPosition(_weaponEntity, XMLoadFloat3(&_moveVector) - XMLoadFloat3(&_moveVector)*amount);
		});


		_builder->Animation()->CreateAnimation(_weaponEntity, "scaleup", 0.25f,
			[this](float delta, float amount, float offset)
		{
			_currentSize += delta;
			_builder->Light()->ChangeLightBlobRange(_weaponEntity, _currentSize);
		});


		_builder->Animation()->CreateAnimation(_weaponEntity, "scaledown", 0.25f,
			[this](float delta, float amount, float offset)
		{
			_currentSize -= delta;
			_builder->Light()->ChangeLightBlobRange(_weaponEntity, _currentSize);
		});

		_builder->Animation()->CreateAnimation(_weaponEntity, "scale", 0.25f,
			[this](float delta, float amount, float offset)
		{
			_currentSize += delta;
			_builder->Light()->ChangeLightBlobRange(_weaponEntity, _currentSize);
		});


		_moveVector = XMFLOAT3(0.0f, 0.0f, 0.0f);
		_currentSize = _eS;
	}
}


Weapon::~Weapon()
{
	for (int i = 0; i < _projectiles.size(); i++)
	{
		delete _projectiles[i];
	}
	_builder->GetEntityController()->ReleaseEntity(_weaponEntity);
}

const vector<Projectile*>& Weapon::GetProjectiles()
{
	return _projectiles;
}

void Weapon::setActive(bool value)
{
	bool prev = _active;
	_active = value;

	if (!prev && _active)
	{
		_builder->Animation()->PlayAnimation(_weaponEntity, "movein", 1.0f);
		_builder->Animation()->PlayAnimation(_weaponEntity, "scaleup", (_currentAmmo / (float)_maxAmmo)*_eS*(1 - _uneS));
	}
	else if (prev && !_active)
	{
		_builder->Animation()->PlayAnimation(_weaponEntity, "moveout", 1.0f);
		_builder->Animation()->PlayAnimation(_weaponEntity, "scaledown", (_currentAmmo / (float)_maxAmmo)*_eS*(1 - _uneS));// - (_currentAmmo / (float)_maxAmmo)*_eS*_uneS);
	}
}

void Weapon::AddAmmo()
{
	float psize = _currentAmmo / (float)_maxAmmo;
	_currentAmmo = _currentAmmo + (unsigned int)(_maxAmmo / 2.0f);
	_maxAmmo += (uint)fmax((int)_currentAmmo - (int)_maxAmmo, 0);
	_builder->Animation()->PlayAnimation(_weaponEntity, "scale", _eS*((_currentAmmo / (float)_maxAmmo - psize)) );
}

