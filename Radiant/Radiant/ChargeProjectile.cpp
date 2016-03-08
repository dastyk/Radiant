#include "ChargeProjectile.h"
#include "System.h"

ChargeProjectile::ChargeProjectile(Entity playerEntity, EntityBuilder* builder, float damageModifier, float radius) : Projectile(builder, playerEntity, damageModifier)
{
	_lifeTime = 4;
	_alive = true;
	_damage = 50 * radius;
	_lightRadius = radius;

	_projectileEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_projectileEntity);

	_builder->Bounding()->CreateBoundingSphere(_projectileEntity, _lightRadius / 8.0f);
	_builder->Light()->BindPointLight(_projectileEntity, XMFLOAT3(0, 0, 0), _lightRadius, XMFLOAT3(0.0f, 0.5f, 0.5f), _lifeTime);
	_builder->Light()->ChangeLightBlobRange(_projectileEntity, _lightRadius);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, _builder->Transform()->GetPosition(playerEntity));
	_builder->GetEntityController()->Transform()->SetPosition(_projectileEntity, temp);
	XMStoreFloat3(&temp, _builder->Transform()->GetRotation(playerEntity));
	_builder->GetEntityController()->Transform()->SetRotation(_projectileEntity, temp);
	_builder->Transform()->MoveForward(_projectileEntity, 0);


}

ChargeProjectile::~ChargeProjectile()
{

}

void ChargeProjectile::Update(float deltaTime)
{
	_lifeTime -= deltaTime;

	if (_lifeTime <= 0.0)
	{
		_alive = false;
	}
	else
	{
		_builder->Transform()->MoveForward(_projectileEntity, 10 * deltaTime);
		_builder->GetEntityController()->Light()->ChangeLightIntensity(_projectileEntity, _lifeTime);
	}
}

bool ChargeProjectile::GetState()
{
	return _alive;
}

void ChargeProjectile::CollideWithEntity(DirectX::XMVECTOR & outMTV, const Entity & entity)
{
	XMVECTOR pos = _builder->Transform()->GetPosition(_projectileEntity) + outMTV*1.1f;
	XMVECTOR rot = _builder->Transform()->GetRotation(_projectileEntity);

	XMFLOAT3 fpos;
	XMStoreFloat3(&fpos, pos);
	XMFLOAT3 frot;
	XMStoreFloat3(&frot, rot);

	Entity de = _builder->CreateDecal(fpos, frot, XMFLOAT3(0.2f, 0.2f, 1.0f),
		"Assets/Textures/badec.png", "Assets/Textures/default_normal.png", "Assets/Textures/badec.png");


	_alive = false;
}
