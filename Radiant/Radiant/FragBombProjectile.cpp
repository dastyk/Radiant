#include "FragBombProjectile.h"
#include "System.h"

FragBombProjectile::FragBombProjectile(Entity playerEntity, EntityBuilder* builder, float damageModifier) : Projectile(builder, playerEntity, damageModifier)
{
	_lifeTime = 1;
	_alive = true;
	_damage = 50.0f;

	_projectileEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_projectileEntity);
	_builder->Bounding()->CreateBoundingSphere(_projectileEntity, 1.0f);
	_builder->Light()->BindPointLight(_projectileEntity, XMFLOAT3(0, 0, 0), 3.0f, XMFLOAT3(1.0f, 0.0f, 1.0f), _lifeTime);
	_builder->Light()->ChangeLightBlobRange(_projectileEntity, 1.0f);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, _builder->Transform()->GetPosition(playerEntity));
	_builder->GetEntityController()->Transform()->SetPosition(_projectileEntity, temp);
	XMStoreFloat3(&temp, _builder->Transform()->GetRotation(playerEntity));
	_builder->GetEntityController()->Transform()->SetRotation(_projectileEntity, temp);
	_builder->Transform()->MoveForward(_projectileEntity, 0);

}

FragBombProjectile::FragBombProjectile(XMFLOAT3 origin, EntityBuilder* builder, float damageModifier) : Projectile(builder, _projectileEntity, damageModifier)
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
	direction.x = (float)(rand() % 361);
	direction.y = (float)(rand() % 361);
	direction.z = (float)(rand() % 361);

	_builder->GetEntityController()->Transform()->SetPosition(_projectileEntity, origin);
	_builder->GetEntityController()->Transform()->SetRotation(_projectileEntity, direction);
	_builder->Transform()->MoveForward(_projectileEntity, 0);

	child = true;

}

void FragBombProjectile::CollideWithEntity(DirectX::XMVECTOR & outMTV, const Entity & entity)
{
		XMVECTOR pos = _builder->Transform()->GetPosition(_projectileEntity) + outMTV*1.1f;
	XMVECTOR rot = _builder->Transform()->GetRotation(_projectileEntity);

	XMFLOAT3 fpos;
	XMStoreFloat3(&fpos, pos);
	XMFLOAT3 frot;
	XMStoreFloat3(&frot, rot);

	Entity de = _builder->CreateDecal(fpos, frot, XMFLOAT3(0.2f, 0.2f, 1.0f),
		"Assets/Textures/fdec.png", "Assets/Textures/default_normal.png", "Assets/Textures/fdec.png");

	_builder->GetEntityController()->Transform()->MoveAlongVector(_projectileEntity, outMTV*5.0f);
	_alive = false;
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
		_builder->GetEntityController()->Light()->ChangeLightIntensity(_projectileEntity, _lifeTime);
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