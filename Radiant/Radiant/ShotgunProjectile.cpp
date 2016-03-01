#include "ShotgunProjectile.h"
#include "System.h"

ShotgunProjectile::ShotgunProjectile(Entity playerEntity, EntityBuilder* builder, float damageModifier) : Projectile(builder, playerEntity, damageModifier)
{
	_lifeTime = 1.0f;
	_alive = true;
	_damage = 35.0f;


	_projectileEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_projectileEntity);
	_builder->Bounding()->CreateBoundingSphere(_projectileEntity, 0.25f*0.3f);
	_builder->Light()->BindPointLight(_projectileEntity, XMFLOAT3(0, 0, 0), 2.5f, XMFLOAT3(0.0f, 1.0f, 0.0f), _lifeTime);
	_builder->Light()->ChangeLightBlobRange(_projectileEntity, 0.25);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, _builder->Transform()->GetPosition(playerEntity));
	_builder->GetEntityController()->Transform()->SetPosition(_projectileEntity, temp);

	XMStoreFloat3(&temp, _builder->Transform()->GetRotation(playerEntity));
	temp.x += (rand() % 3001 - 2000) / 100.0f;
	temp.y += (rand() % 3001 - 2000) / 100.0f;
	temp.z += (rand() % 3001 - 2000) / 100.0f;
	_builder->GetEntityController()->Transform()->SetRotation(_projectileEntity, temp);
	_builder->Transform()->MoveForward(_projectileEntity, 0);

}

ShotgunProjectile::~ShotgunProjectile()
{
	
}

void ShotgunProjectile::Update(float deltaTime)
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

bool ShotgunProjectile::GetState()
{
	return _alive;
}