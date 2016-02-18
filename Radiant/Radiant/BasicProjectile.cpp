#include "BasicProjectile.h"
#include "System.h"

BasicProjectile::BasicProjectile(Entity playerEntity, EntityBuilder* builder) : Projectile(builder)
{
	_lifeTime = 2;
	_alive = true;
	_damage = 35.0f;

	_projectileEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_projectileEntity);

	_builder->Bounding()->CreateBoundingSphere(_projectileEntity, 2.0f);
	_builder->Light()->BindPointLight(_projectileEntity, XMFLOAT3(0, 0, 0), 2, XMFLOAT3(0.0f, 0.0f, 1.0f), 10);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, _builder->Transform()->GetPosition(playerEntity));
	_builder->GetEntityController()->Transform()->SetPosition(_projectileEntity, temp);
	XMStoreFloat3(&temp, _builder->Transform()->GetRotation(playerEntity));
	_builder->GetEntityController()->Transform()->SetRotation(_projectileEntity, temp);
	_builder->Transform()->MoveForward(_projectileEntity, 0);
	
}

BasicProjectile::~BasicProjectile()
{

}

void BasicProjectile::Update(float deltaTime)
{
	_lifeTime -= deltaTime;

	if (_lifeTime <= 0.0)
	{
		_alive = false;
	}
	else
	{
		_builder->Transform()->MoveForward(_projectileEntity, 20 * deltaTime);
	}
}

bool BasicProjectile::GetState()
{
	return _alive;
}