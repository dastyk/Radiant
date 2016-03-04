#include "Weapon.h"

Weapon::Weapon(EntityBuilder* builder, unsigned int type) : _builder(builder),_type(type)
{
	_weaponEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_weaponEntity);

	_builder->Animation()->CreateAnimation(_weaponEntity, "moveout", 0.25f,
		[this](float delta, float amount, float offset)
	{
		_builder->Transform()->MoveAlongVector(_weaponEntity, XMLoadFloat3(&_moveVector)*delta);
	});

	_builder->Animation()->CreateAnimation(_weaponEntity, "movein", 0.25f,
		[this](float delta, float amount, float offset)
	{
		_builder->Transform()->MoveAlongVector(_weaponEntity, XMLoadFloat3(&_moveVector)*delta*-1.0f);
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
	_currentSize = 0.1f;
}

Weapon::~Weapon()
{
	for (int i = 0; i < _projectiles.size(); i++)
	{
		delete _projectiles[i];
	}
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
		_builder->Animation()->PlayAnimation(_weaponEntity, "movein", 0.06f);
		_builder->Animation()->PlayAnimation(_weaponEntity, "scaleup", 0.1f*(_currentAmmo / (float)_maxAmmo) - 0.05f);
	}
	else if (prev && !_active)
	{
		_builder->Animation()->PlayAnimation(_weaponEntity, "moveout", 0.06f);
		_builder->Animation()->PlayAnimation(_weaponEntity, "scaledown", 0.1f*((_currentAmmo / (float)_maxAmmo)) -  0.05f);
	}
}

void Weapon::AddAmmo()
{
	float psize = _currentAmmo / (float)_maxAmmo;
	_currentAmmo = _currentAmmo + (unsigned int)(_maxAmmo / 2.0f);
	_maxAmmo += (uint)fmax((int)_currentAmmo - (int)_maxAmmo, 0);
	_builder->Animation()->PlayAnimation(_weaponEntity, "scale", 0.1f*((_currentAmmo / (float)_maxAmmo) - psize) );
}