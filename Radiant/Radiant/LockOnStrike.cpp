#include "LockOnStrike.h"
#include "Shodan.h"
#include "System.h"

LockOnStrike::LockOnStrike(EntityBuilder* builder, Entity player, List<EnemyWithStates>* _Entities) : Power(builder)
{
	_timeSinceLastActivation = 100;
	_cooldown = 10.0;
	_active = true;
	_enemies = _Entities;
	_damage = 50.0f;
	_maxNumbersOfProjectiles = 3;
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

	if (_active)
	{
		if (System::GetInput()->IsMouseKeyPushed(VK_RBUTTON))
		{
			if (_cooldown - _timeSinceLastActivation <= 0.0f  && _projectiles.size() <= _maxNumbersOfProjectiles)
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
		}
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

			for (int j = 0; j < _enemies->Size(); j++)
			{
				if (XMVectorGetX(XMVector3Length(XMVectorSet(temp.x - _enemies->GetCurrentElement()->_thisEnemy->GetCurrentPos().x, temp.y - _enemies->GetCurrentElement()->_thisEnemy->GetCurrentPos().y, temp.z - _enemies->GetCurrentElement()->_thisEnemy->GetCurrentPos().z, 0))) <= 3.0f)
				{
					j = _enemies->Size();

					XMFLOAT3 projRot, projPos, enemyPos, direction;
					XMStoreFloat3(&projRot, _builder->Transform()->GetRotation(_projectiles[i]));
					XMStoreFloat3(&projPos, _builder->Transform()->GetPosition(_projectiles[i]));
					XMStoreFloat3(&enemyPos, _builder->Transform()->GetPosition(_enemies->GetCurrentElement()->_thisEnemy->GetEntity()));


					XMStoreFloat3(&direction, XMVector3AngleBetweenVectors(XMVectorSet(projRot.x, 0, projRot.z, 1), XMVectorSet(enemyPos.x - projPos.x, 0, enemyPos.z - projPos.z, 1)));
					_builder->Transform()->SetRotation(_projectiles[i], direction);
					_foundTarget[i] = true;

				}
				_enemies->MoveCurrent();
			}
		}
		else
		{
			float minDistance = 10000;
			XMFLOAT3 closestEnemy, currentEnemy;
			float temp = 0.0f;

			XMFLOAT3 projPos;
			XMStoreFloat3(&projPos, _builder->Transform()->GetPosition(_projectiles[i]));

			if (_enemies->Size() == 0)
			{
				_builder->GetEntityController()->ReleaseEntity(_projectiles[i]);
				_projectiles.erase(_projectiles.begin() + i);
				_angles.erase(_angles.begin() + i);
				_foundTarget.erase(_foundTarget.begin() + i);
			}

			for (int j = 0; j < _enemies->Size(); j++)
			{
				if (_builder->Bounding()->CheckCollision(_projectiles[i], _enemies->GetCurrentElement()->_thisEnemy->GetEntity()))
				{
					_builder->GetEntityController()->ReleaseEntity(_projectiles[i]);
					_projectiles.erase(_projectiles.begin() + i);
					_angles.erase(_angles.begin() + i);
					_foundTarget.erase(_foundTarget.begin() + i);
					_enemies->RemoveCurrentElement();
				}
				else
				{

					XMStoreFloat3(&currentEnemy, _builder->Transform()->GetPosition(_enemies->GetCurrentElement()->_thisEnemy->GetEntity()));

					temp = XMVectorGetX(XMVector3Length(XMVectorSet(currentEnemy.x - projPos.x, currentEnemy.y - projPos.y, currentEnemy.z - projPos.z, 0)));

					if (temp < minDistance)
					{
						minDistance = temp;
						XMStoreFloat3(&closestEnemy, _builder->Transform()->GetPosition(_enemies->GetCurrentElement()->_thisEnemy->GetEntity()));
					}

					_enemies->MoveCurrent();

				}
			}

			XMVECTOR moveVec;

			// Ta ut vector mellan
			moveVec = XMLoadFloat3(&XMFLOAT3(closestEnemy.x - projPos.x, closestEnemy.y - projPos.y, closestEnemy.z - projPos.z));
			
			XMVector3Normalize(moveVec);

			_builder->Transform()->MoveAlongVector(_projectiles[i], moveVec * 2 * deltaTime);
			
		}


	}
}

void LockOnStrike::Upgrade()
{

}