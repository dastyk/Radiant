#include "FragBombProjectile.h"
#include "System.h"

FragBombProjectile::FragBombProjectile(Entity playerEntity, EntityBuilder* builder) : Projectile(builder)
{
	_lifeTime = 1;
	_alive = true;
	_damage = 50.0f;

	_projectileEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_projectileEntity);
	_builder->Bounding()->CreateBoundingSphere(_projectileEntity, 1.0f);
	_builder->Light()->BindPointLight(_projectileEntity, XMFLOAT3(0, 0, 0), 1.0f, XMFLOAT3(1.0f, 0.0f, 1.0f), 10);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, _builder->Transform()->GetPosition(playerEntity));
	_builder->GetEntityController()->Transform()->SetPosition(_projectileEntity, temp);
	XMStoreFloat3(&temp, _builder->Transform()->GetRotation(playerEntity));
	_builder->GetEntityController()->Transform()->SetRotation(_projectileEntity, temp);
	_builder->Transform()->MoveForward(_projectileEntity, 0);

}

FragBombProjectile::FragBombProjectile(XMFLOAT3 origin, EntityBuilder* builder) : Projectile(builder)
{
	_lifeTime = 2;
	_alive = true;
	_damage = 20.0f;

	// Create dummy model for collision -- Needs to be changed if starting camera position changes -- Also should probably look at near plane position and adjust to avoid clipping

	_projectileEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_projectileEntity);
	_builder->Bounding()->CreateBoundingSphere(_projectileEntity, 0.5f);
	_builder->Light()->BindPointLight(_projectileEntity, XMFLOAT3(0, 0, 0), 0.5, XMFLOAT3(1.0f, 105.0f / 255.0f, 180.0f / 255.0f), 10);

	XMFLOAT3 direction;
	direction.x = (rand() % 361);
	direction.y = (rand() % 361);
	direction.z = (rand() % 361);

	_builder->GetEntityController()->Transform()->SetPosition(_projectileEntity, origin);
	_builder->GetEntityController()->Transform()->SetRotation(_projectileEntity, direction);
	_builder->Transform()->MoveForward(_projectileEntity, 0);

	child = true;

}

FragBombProjectile::~FragBombProjectile()
{

}

void FragBombProjectile::Update(float deltaTime)
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

bool FragBombProjectile::GetState()
{
	return _alive;
}

bool FragBombProjectile::IsChild()
{
	return child;
}