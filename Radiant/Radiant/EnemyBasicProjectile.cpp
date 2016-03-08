#include "EnemyBasicProjectile.h"
#include "System.h"

EnemyBasicProjectile::EnemyBasicProjectile(Entity enemyEntity, EntityBuilder* builder, XMFLOAT3 parentColor, XMFLOAT3 &playerPosition, float damageModifier) : Projectile(builder, enemyEntity, damageModifier)
{
	_lifeTime = 5.2f;
	_alive = true;
	_damage = 2.5f;

	_projectileEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_projectileEntity);

	_builder->Bounding()->CreateBoundingSphere(_projectileEntity, 0.05f);
	_builder->Light()->BindPointLight(_projectileEntity, XMFLOAT3(0, 0, 0), 0.15f, XMFLOAT3(parentColor.x/1.3f, parentColor.y*2.3f, parentColor.z/0.7f), 15.2f);
	_builder->Light()->ChangeLightRange(_projectileEntity, 1.80f);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, _builder->Transform()->GetPosition(enemyEntity));
	_builder->GetEntityController()->Transform()->SetPosition(_projectileEntity, temp);

	XMStoreFloat3(&_movementVector, XMVector3Normalize(XMVectorSet(playerPosition.x - temp.x, playerPosition.y - temp.y, playerPosition.z - temp.z, 0.0f)));

	_builder->Transform()->MoveForward(_projectileEntity, 0);


}

EnemyBasicProjectile::~EnemyBasicProjectile()
{

}

void EnemyBasicProjectile::Update(float deltaTime)
{
	_lifeTime -= deltaTime;

	if (_lifeTime <= 0.0)
	{
		_alive = false;
	}
	else
	{
		_builder->Transform()->MoveAlongVector(_projectileEntity, XMLoadFloat3(&_movementVector)*2*deltaTime);
	}
}

bool EnemyBasicProjectile::GetState()
{
	return _alive;
}