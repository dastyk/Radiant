#include "EnemyMiniGunProjectile.h"
#include "System.h"

EnemyMiniGunProjectile::EnemyMiniGunProjectile(Entity enemyEntity, EntityBuilder* builder, XMFLOAT3 parentColor, XMFLOAT3 &playerPosition, float damageModifier) : Projectile(builder, enemyEntity, damageModifier)
{
	_lifeTime = 15.2f;
	_alive = true;
	_damage = 2.0f;

	_projectileEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_projectileEntity);

	_builder->Bounding()->CreateBoundingSphere(_projectileEntity, 0.35f);
	_builder->Light()->BindPointLight(_projectileEntity, XMFLOAT3(0, 0, 0), 0.35f, XMFLOAT3(parentColor.x*0.0f, parentColor.y*0.28f, parentColor.z*1.7f), _lifeTime);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, _builder->Transform()->GetPosition(enemyEntity));
	_builder->GetEntityController()->Transform()->SetPosition(_projectileEntity, temp);

	XMStoreFloat3(&_movementVector, XMVector3Normalize(XMVectorSet(playerPosition.x - temp.x, playerPosition.y - temp.y, playerPosition.z - temp.z, 0.0f)));

}

EnemyMiniGunProjectile::~EnemyMiniGunProjectile()
{

}

void EnemyMiniGunProjectile::Update(float deltaTime)
{
	_lifeTime -= deltaTime;

	if (_lifeTime <= 0.0)
	{
		_alive = false;
	}
	else
	{
		_builder->Transform()->MoveAlongVector(_projectileEntity, XMLoadFloat3(&_movementVector) *9* deltaTime);
		_builder->GetEntityController()->Light()->ChangeLightIntensity(_projectileEntity, _lifeTime);
	}
}

bool EnemyMiniGunProjectile::GetState()
{
	return _alive;
}