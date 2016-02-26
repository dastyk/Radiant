#include "EnemyDeathBlossomProjectile.h"
#include "System.h"

EnemyDeathBlossomProjectile::EnemyDeathBlossomProjectile(Entity enemyEntity, EntityBuilder* builder, XMFLOAT3 parentColor, XMFLOAT3 &playerPosition, float angle) : Projectile(builder)
{
	_lifeTime = 2.2;
	_alive = true;
	_damage = 10.5f;

	_projectileEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_projectileEntity);

	_builder->Bounding()->CreateBoundingSphere(_projectileEntity, 0.35f);
	_builder->Light()->BindPointLight(_projectileEntity, XMFLOAT3(0, 0, 0), 0.35f, XMFLOAT3(parentColor.x*0.53f, parentColor.y*0.28f, parentColor.z*1.7f), _lifeTime);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, _builder->Transform()->GetPosition(enemyEntity));
	_builder->GetEntityController()->Transform()->SetPosition(_projectileEntity, temp);

	angle *= XM_PI / 180;

	XMStoreFloat3(&_movementVector, XMVector3Normalize(XMVector3Rotate(XMVectorSet(playerPosition.x - temp.x, 0.0f, playerPosition.z - temp.z, 0.0f), XMQuaternionRotationNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), angle))));
	
}

EnemyDeathBlossomProjectile::~EnemyDeathBlossomProjectile()
{

}

void EnemyDeathBlossomProjectile::Update(float deltaTime)
{
	_lifeTime -= deltaTime;

	if (_lifeTime <= 0.0)
	{
		_alive = false;
	}
	else
	{
		_builder->Transform()->MoveAlongVector(_projectileEntity, XMLoadFloat3(&_movementVector) * deltaTime);
		_builder->GetEntityController()->Light()->ChangeLightIntensity(_projectileEntity, _lifeTime);
	}
}

bool EnemyDeathBlossomProjectile::GetState()
{
	return _alive;
}