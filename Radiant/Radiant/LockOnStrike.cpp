#include "LockOnStrike.h"
#include "Shodan.h"
#include "System.h"

LockOnStrike::LockOnStrike(EntityBuilder* builder, Entity player, List<EnemyWithStates>* _Entities) : Power(builder)
{
	_timeSinceLastActivation = 100;
	_cooldown = 10.0;
	_active = true;
	_enemies = _Entities;
}

LockOnStrike::~LockOnStrike()
{
	_builder->GetEntityController()->ReleaseEntity(_powerEntity);

	for (int i = 0; i < _projectiles.size(); i++)
	{
		_builder->GetEntityController()->ReleaseEntity(_projectiles[i]);
	}
}

void LockOnStrike::Update(Entity playerEntity, float deltaTime)
{
	_timeSinceLastActivation += deltaTime;

	if (_cooldown - _timeSinceLastActivation <= 0.0f && System::GetInput()->IsMouseKeyPushed(VK_RBUTTON) && _active && _projectiles.size() <= 3)
	{
		_timeSinceLastActivation = 0.0f;
		
		Entity ent = _builder->EntityC().Create();
		_builder->Transform()->CreateTransform(ent);
		_builder->Bounding()->CreateBoundingSphere(ent, 0.5f);

		_builder->Light()->BindPointLight(ent, XMFLOAT3(0, 0, 0), 0.5f, XMFLOAT3(1.0f, 0.0f, 0.0f), 100.0f);
		_builder->Transform()->MoveDown(ent, 0.0f);


		_projectiles.push_back(ent);
		_angles.push_back(0);
		_foundTarget.push_back(false);
	}

	_MoveProjectiles(playerEntity, deltaTime);

}

void LockOnStrike::_MoveProjectiles(Entity playerEntity, float deltaTime)
{
	XMFLOAT3 playerPos;
	
	XMStoreFloat3(&playerPos, _builder->Transform()->GetPosition(playerEntity));

	for (int i = 0; i < _projectiles.size(); i++)
	{
		if (!_foundTarget[i])
		{
			XMFLOAT3 temp;

			temp.x = playerPos.x + cos(_angles[i]);
			temp.y = playerPos.y - 0.25f;
			temp.z = playerPos.z + sin(_angles[i]);

			_angles[i] += (XM_PI * deltaTime);
			if (_angles[i] >= XM_2PI)
			{
				_angles[i] = 0.0f;
			}

			_builder->Transform()->SetPosition(_projectiles[i], temp);

			//for (int j = 0; j < _enemies->Size(); j++)
			//{
			//	if (XMVectorGetX(XMVector3Length(XMVectorSet(temp.x - _enemies->GetCurrentElement()->_thisEnemy->GetCurrentPos().x, temp.y - _enemies->GetCurrentElement()->_thisEnemy->GetCurrentPos().y, temp.z - _enemies->GetCurrentElement()->_thisEnemy->GetCurrentPos().z, 0))) <= 3.0f)
			//	{
			//		j = _enemies->Size();

			//		XMFLOAT3 projRot, projPos, enemyPos, direction;
			//		XMStoreFloat3(&projRot, _builder->Transform()->GetRotation(_projectiles[i]));
			//		XMStoreFloat3(&projPos, _builder->Transform()->GetPosition(_projectiles[i]));
			//		XMStoreFloat3(&enemyPos, _builder->Transform()->GetPosition(_enemies->GetCurrentElement()->_thisEnemy->GetEntity()));


			//		XMStoreFloat3(&direction, XMVector3AngleBetweenVectors(XMVectorSet(projRot.x, 0, projRot.z, 1), XMVectorSet(enemyPos.x - projPos.x, 0, enemyPos.z - projPos.z, 1)));
			//		_builder->Transform()->SetRotation(_projectiles[i], direction);

			//	}
			//	_enemies->MoveCurrent();
			//}
		}
		else
		{
			_builder->Transform()->MoveForward(_projectiles[i], 20 * deltaTime);
		}


	}
}