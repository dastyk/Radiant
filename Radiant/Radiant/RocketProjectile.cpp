#include "RocketProjectile.h"
#include "System.h"

RocketProjectile::RocketProjectile(Entity playerEntity, EntityBuilder* builder, float damageModifier) : Projectile(builder, playerEntity, damageModifier)
{
	_lifeTime = 3.0f;
	_alive = true;
	_damage = 40.0f;

	_projectileEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_projectileEntity);
	_builder->Bounding()->CreateBoundingSphere(_projectileEntity, 0.05f);
	_builder->Light()->BindPointLight(_projectileEntity, XMFLOAT3(0, 0, 0), 5.0f, XMFLOAT3(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f), _lifeTime);
	_builder->Light()->ChangeLightBlobRange(_projectileEntity, 0.25);

	for (int i = 0; i < 3; i++)
	{
		_tails[i] = _builder->EntityC().Create();

		_builder->Transform()->CreateTransform(_tails[i]);
		_builder->Bounding()->CreateBoundingSphere(_tails[i], 0.05f);
		_builder->Light()->BindPointLight(_tails[i], XMFLOAT3(0, 0, 0), 2.5f, XMFLOAT3(1.0f, 215.0f / 255.0f, 0.0f), _lifeTime);
		_builder->Light()->ChangeLightBlobRange(_tails[i], 0.125);

		_builder->Transform()->BindChild(_projectileEntity, _tails[i]);

		_builder->Transform()->SetPosition(_tails[i], XMFLOAT3(sin((XM_2PI / 3.0f) * i) * 0.05f, cos((XM_2PI / 3.0f) * i) * 0.05f, -0.05f));
	}

	XMFLOAT3 temp;
	XMStoreFloat3(&temp, _builder->Transform()->GetPosition(playerEntity));
	_builder->GetEntityController()->Transform()->SetPosition(_projectileEntity, temp);
	XMStoreFloat3(&temp, _builder->Transform()->GetRotation(playerEntity));
	_builder->GetEntityController()->Transform()->SetRotation(_projectileEntity, temp);
	_builder->Transform()->MoveForward(_projectileEntity, 0);

}

RocketProjectile::~RocketProjectile()
{
	for (int i = 0; i < 3; i++)
		_builder->GetEntityController()->ReleaseEntity(_tails[i]);
}

void RocketProjectile::Update(float deltaTime)
{
	_lifeTime -= deltaTime;

	if (_lifeTime <= 0.0)
	{
		_alive = false;
	}
	else
	{
		_builder->Transform()->MoveForward(_projectileEntity, (0.125f + pow((3 - _lifeTime), 4)) * deltaTime);
		_builder->GetEntityController()->Light()->ChangeLightIntensity(_projectileEntity, _lifeTime);
		_damage = 40.0f + (3.0f - _lifeTime) * 30;

		_builder->Transform()->RotateRoll(_projectileEntity, 6 * XM_2PI * deltaTime * pow((3 - _lifeTime), 4));
		
	}
}

bool RocketProjectile::GetState()
{
	return _alive;
}

void RocketProjectile::CollideWithEntity(DirectX::XMVECTOR & outMTV, const Entity & entity)
{
	XMVECTOR pos = _builder->Transform()->GetPosition(_projectileEntity) + outMTV*1.1f;
	XMVECTOR rot = _builder->Transform()->GetRotation(_projectileEntity);

	XMFLOAT3 fpos;
	XMStoreFloat3(&fpos, pos);
	XMFLOAT3 frot;
	XMStoreFloat3(&frot, rot);

	Entity de = _builder->CreateDecal(fpos, frot, XMFLOAT3(0.2f, 0.2f, 1.0f),
		"Assets/Textures/rdec.png", "Assets/Textures/default_normal.png", "Assets/Textures/rdec.png");

	_alive = false;
}
