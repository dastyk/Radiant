#include "RapidFireProjectile.h"
#include "System.h"

RapidFireProjectile::RapidFireProjectile(Entity playerEntity, EntityBuilder* builder) : Projectile(builder)
{
	_lifeTime = 1.5f;
	_alive = true;
	_damage = 10.0f;

	_projectileEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_projectileEntity);
	_builder->Bounding()->CreateBoundingSphere(_projectileEntity, 0.25f);
	_builder->Light()->BindPointLight(_projectileEntity, XMFLOAT3(0, 0, 0), 0.25f, XMFLOAT3(1.0f, 0.0f, 0.0f), _lifeTime);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, _builder->Transform()->GetPosition(playerEntity));
	_builder->GetEntityController()->Transform()->SetPosition(_projectileEntity, temp);
	XMStoreFloat3(&temp, _builder->Transform()->GetRotation(playerEntity));
	_builder->GetEntityController()->Transform()->SetRotation(_projectileEntity, temp);
	_builder->Transform()->MoveForward(_projectileEntity, 0);

}

RapidFireProjectile::~RapidFireProjectile()
{

}

void RapidFireProjectile::Update(float deltaTime)
{
	_lifeTime -= deltaTime;

	if (_lifeTime <= 0.0)
	{
		_alive = false;
	}
	else
	{
		_builder->Transform()->MoveForward(_projectileEntity, 20 * deltaTime);
		_builder->GetEntityController()->Light()->ChangeLightIntensity(_projectileEntity, _lifeTime);
	}
}

bool RapidFireProjectile::GetState()
{
	return _alive;
}