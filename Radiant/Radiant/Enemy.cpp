#include "Enemy.h"

Enemy::Enemy()
{

}

Enemy::Enemy(Entity enemyEntity, EntityBuilder* builder) : _builder(builder), _myPath(nullptr)
{
	_enemyEntity = enemyEntity;
	_myPath = nullptr;
	_movementVector = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_speedFactor = 2.0f;
	_nrOfStepsTaken = 0;
	_myColor = _builder->Light()->GetLightColor(enemyEntity);

	_timeSinceLastSound = 100;
}

Enemy::~Enemy()
{
	SAFE_DELETE(_myPath);
	SAFE_DELETE(_weapon);
	_builder->GetEntityController()->ReleaseEntity(_enemyEntity);
}

Entity Enemy::GetEntity()
{
	return _enemyEntity;
}

void Enemy::Update(float deltaTime)
{
	_builder->GetEntityController()->Transform()->RotateYaw(_rotation, deltaTime*120);
	_timeSinceLastSound += deltaTime;
}

bool Enemy::UpdateMovement(float deltaTime)
{
	XMVECTOR move = XMLoadFloat3(&_movementVector);
	if (_waiting)
	{
		_waitTime -= deltaTime;
		if (_waitTime < 0.0f)
		{
			_waiting = false;
		}
	}
	if (!_myPath)
		return false;
	_timer += deltaTime;
	if (_timer > 0.02f)
	{
		XMVECTOR test = _builder->Transform()->GetPosition(_enemyEntity);
		move = XMVectorSetX(move, _currentGoal.x - XMVectorGetX(test));
		move = XMVectorSetZ(move, _currentGoal.z - XMVectorGetZ(test));
		move = XMVector3Normalize(move);
		XMStoreFloat3(&_movementVector, move);
		_timer = 0.00f;
	}
	_builder->Transform()->MoveAlongVector(_enemyEntity, move*deltaTime);
	XMVECTOR test = XMVectorSetY(_builder->Transform()->GetPosition(_enemyEntity), 0.0f);
	float testValue = XMVectorGetX(XMVector3Length(test - XMLoadFloat3(&_currentGoal)));
	if (testValue < 0.01)
	{
		int didIChange = rand() % 1000;
		if (didIChange == 0)
		{
			testValue = 0.0f;
		}
	}
	if (testValue < 0.007f)
	{
		XMVECTOR test = _builder->Transform()->GetPosition(_enemyEntity);
		if (_nrOfStepsTaken != _myPath->nrOfNodes)
		{
			_nrOfStepsTaken++;
			_currentGoal = XMFLOAT3(_myPath->nodes[_nrOfStepsTaken].x - 0.5f, 0, _myPath->nodes[_nrOfStepsTaken].y - 0.5f);
			move = XMVectorSetX(move, _currentGoal.x - XMVectorGetX(test));
			move = XMVectorSetY(move, 0.5f - XMVectorGetY(test));
			move = XMVectorSetZ(move, _currentGoal.z - XMVectorGetZ(test));
			move = XMVector3Normalize(move);
			XMStoreFloat3(&_movementVector, move);
		}
		else
		{
			_nrOfStepsTaken++;
		}
		
	}

	return true;

}

void Enemy::Attack(float deltaTime, XMVECTOR playerPosition)
{
	SAFE_DELETE(_myPath);
	XMVECTOR playerMinusY = XMVectorSetY(playerPosition, 0.0f);
	XMVECTOR entityMinusY = XMLoadFloat3(&GetCurrentPos());
	entityMinusY = XMVectorSetY(entityMinusY, 0.0f);
	float lengthToPlayer = XMVectorGetX(XMVector3Length(entityMinusY - playerMinusY));
	if (lengthToPlayer > 1.5f)
	{
		XMVECTOR move = XMVector3Normalize(playerMinusY - entityMinusY);

		XMStoreFloat3(&_movementVector, move);

		_builder->Transform()->MoveAlongVector(_enemyEntity, move*deltaTime);
	}



}

void Enemy::GivePath(Path* newPath)
{
	SAFE_DELETE(_myPath);
	_myPath = newPath;

	XMVECTOR move = XMLoadFloat3(&_movementVector);
	XMVECTOR temp = _builder->Transform()->GetPosition(_enemyEntity);
	if (_myPath->nrOfNodes)
	{
		_nrOfStepsTaken = 0;
		_currentGoal = XMFLOAT3(_myPath->nodes[_nrOfStepsTaken].x - 0.5f, 0, _myPath->nodes[_nrOfStepsTaken].y - 0.5f);
		move = XMVectorSetX(move, _currentGoal.x - XMVectorGetX(temp));
		move = XMVectorSetZ(move, _currentGoal.z - XMVectorGetZ(temp));
		move = XMVector3Normalize(move);
		XMStoreFloat3(&_movementVector, move);
		_waitTime = rand() % 10000;
		_waiting = true;
	}
}

void Enemy::SetSpeedFactor(float factor)
{
	_speedFactor = factor;
}

bool Enemy::Walking()
{
	if (!_myPath)
	{	//Very, very, VERY ugly fix for now
		_currentGoal.y = 0.5f;
		_currentGoal.z += 0.5f;
		_currentGoal.x += 0.5f;
		_builder->Transform()->SetPosition(_enemyEntity, XMLoadFloat3(&_currentGoal));
		return true;
	}

	if (_nrOfStepsTaken <= _myPath->nrOfNodes)
	{
		return true;
	}
	////"Allign it" to the Node system.
	_currentGoal.y = 0.5f;
	_currentGoal.z += 0.5f;
	_currentGoal.x += 0.5f;
	_builder->Transform()->SetPosition(_enemyEntity, XMLoadFloat3(&_currentGoal));
	return false;
}

XMFLOAT3 Enemy::GetCurrentPos()
{
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, _builder->Transform()->GetPosition(_enemyEntity));
	return temp;
}

float Enemy::ReduceHealth(float amount)
{
	_health -= amount;
	return _health;
}

float Enemy::GetHealth()
{
	return _health;
}

float Enemy::GetTimeSinceLastSound()
{
	return _timeSinceLastSound;
}

void Enemy::ResetTimeSinceLastSound()
{
	_timeSinceLastSound = 0;
}

void Enemy::SetCurrentGoal(XMFLOAT3 currentGoal)
{
	_currentGoal = currentGoal;
}

XMFLOAT3 Enemy::GetCurrentGoal()
{
	return _currentGoal;
}

vector<Projectile*> Enemy::GetProjectiles()
{
	return _weapon->GetProjectiles();
}

EnemyWeapon* Enemy::GetWeapon()
{
	return _weapon;
}

void Enemy::SetCurrentWeapon(EnemyWeapon* myWeapon)
{
	SAFE_DELETE(_weapon);
	_weapon = myWeapon;
}

XMFLOAT3 Enemy::GetColor()
{
	return _myColor;
}