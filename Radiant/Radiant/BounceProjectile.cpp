#include "BounceProjectile.h"
#include "System.h"

BounceProjectile::BounceProjectile(Entity playerEntity, EntityBuilder* builder) : Projectile(builder)
{
	_lifeTime = 5;
	_alive = true;
	_damage = 20.0f;
	_owner = PLAYER_OWNER;

	_projectileEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_projectileEntity);

	_builder->Bounding()->CreateBoundingSphere(_projectileEntity, 0.5f*0.3f);
	_builder->Light()->BindPointLight(_projectileEntity, XMFLOAT3(0, 0, 0), 3.0f, XMFLOAT3(1.0f, 165.0f / 255.0f, 0.0f), _lifeTime);
	_builder->Light()->ChangeLightBlobRange(_projectileEntity, 0.5);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, _builder->Transform()->GetPosition(playerEntity));
	_builder->GetEntityController()->Transform()->SetPosition(_projectileEntity, temp);
	XMStoreFloat3(&temp, _builder->Transform()->GetRotation(playerEntity));
	_builder->GetEntityController()->Transform()->SetRotation(_projectileEntity, temp);
	_builder->Transform()->MoveForward(_projectileEntity, 0);


}

BounceProjectile::~BounceProjectile()
{

}

void BounceProjectile::Update(float deltaTime)
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

bool BounceProjectile::GetState()
{
	return _alive;
}

void BounceProjectile::CollideWithEntity(DirectX::XMVECTOR& outMTV, const Entity& entity)
{
	XMVECTOR directionVector = _builder->Transform()->GetDirection(_projectileEntity);
	XMVECTOR normal = XMVector3Normalize(outMTV);
	_builder->Transform()->MoveAlongVector(_projectileEntity, normal);

	if (XMVectorGetZ(normal))
		int apa = 0;

	XMVECTOR bouncedVector = XMVector3Reflect(directionVector, normal);
	_builder->Transform()->SetDirection(_projectileEntity, bouncedVector);
}