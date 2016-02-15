#include "Enemy.h"

Enemy::Enemy()
{

}

Enemy::Enemy(Entity enemyEntity, EntityBuilder* builder) : _builder(builder)
{
	_enemyEntity = enemyEntity;
	_myPath = nullptr;
	_movementVector = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_speedFactor = 2.0f;
	_nrOfStepsTaken = 0;



	// Create dummy model for collision -- Needs to be changed if starting camera position changes -- Also should probably look at near plane position and adjust to avoid clipping

	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT2> uv;
	std::vector<unsigned int> indices;

	positions.resize(16);
	uv.resize(16);
	indices.resize(24);

	float extent = 0.1f;

	for (int i = 0; i < 4; i++) // front face
	{
		if (i == 0 || i == 2)
		{
			positions[i].x = 0 - extent;
			positions[i].y = i * extent - extent;
			positions[i].z = 0 - extent;
		}
		else if (i == 1 || i == 3)
		{
			positions[i].x = 0 - extent;
			positions[i].y = (i - 1) * extent - extent;
			positions[i].z = 0 + extent;
		}
	}

	uv[0].x = 1;
	uv[0].y = 1;

	uv[1].x = 0;
	uv[1].y = 1;

	uv[2].x = 1;
	uv[2].y = 0;

	uv[3].x = 0;
	uv[3].y = 0;


	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;

	indices[3] = 1;
	indices[4] = 2;
	indices[5] = 3;


	for (int i = 0; i < 4; i++) // back face
	{
		if (i == 0 || i == 2)
		{
			positions[i + 4].x = 0 + extent;
			positions[i + 4].y = i * extent - extent;
			positions[i + 4].z = 0 - extent;
		}
		else if (i == 1 || i == 3)
		{
			positions[i + 4].x = 0 + extent;
			positions[i + 4].y = (i - 1) * extent - extent;
			positions[i + 4].z = 0 + extent;
		}
	}


	uv[4].x = 0;
	uv[4].y = 1;

	uv[5].x = 1;
	uv[5].y = 1;

	uv[6].x = 0;
	uv[6].y = 0;

	uv[7].x = 1;
	uv[7].y = 0;


	indices[6] = 1 + 4;
	indices[7] = 2 + 4;
	indices[8] = 0 + 4;

	indices[9] = 3 + 4;
	indices[10] = 2 + 4;
	indices[11] = 1 + 4;


	for (int i = 0; i < 4; i++) // left face
	{
		if (i == 0 || i == 2)
		{
			positions[i + 8].x = 0 + extent;
			positions[i + 8].y = i * extent - extent;
			positions[i + 8].z = 0 - extent;
		}
		else if (i == 1 || i == 3)
		{
			positions[i + 8].x = 0 - extent;
			positions[i + 8].y = (i - 1) * extent - extent;
			positions[i + 8].z = 0 - extent;
		}
	}


	uv[8].x = 1;
	uv[8].y = 1;

	uv[9].x = 0;
	uv[9].y = 1;

	uv[10].x = 1;
	uv[10].y = 0;

	uv[11].x = 0;
	uv[11].y = 0;


	indices[12] = 0 + 8;
	indices[13] = 2 + 8;
	indices[14] = 1 + 8;

	indices[15] = 1 + 8;
	indices[16] = 2 + 8;
	indices[17] = 3 + 8;


	for (int i = 0; i < 4; i++) // right face
	{
		if (i == 0 || i == 2)
		{
			positions[i + 12].x = 0 - extent;
			positions[i + 12].y = i * extent - extent;
			positions[i + 12].z = 0 + extent;
		}
		else if (i == 1 || i == 3)
		{
			positions[i + 12].x = 0 + extent;
			positions[i + 12].y = (i - 1) * extent - extent;
			positions[i + 12].z = 0 + extent;
		}
	}


	uv[12].x = 1;
	uv[12].y = 1;

	uv[13].x = 0;
	uv[13].y = 1;

	uv[14].x = 1;
	uv[14].y = 0;

	uv[15].x = 0;
	uv[15].y = 0;


	indices[18] = 0 + 12;
	indices[19] = 2 + 12;
	indices[20] = 1 + 12;

	indices[21] = 1 + 12;
	indices[22] = 2 + 12;
	indices[23] = 3 + 12;


	_builder->Mesh()->CreateStaticMesh(_enemyEntity, "Enemy", positions, uv, indices);
	_builder->Mesh()->ToggleVisibility(_enemyEntity, 0);
	_builder->Material()->BindMaterial(_enemyEntity, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_enemyEntity, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_enemyEntity, 0, "Metallic", 0.1f, "Shaders/GBuffer.hlsl");
	_builder->Bounding()->CreateBoundingBox(_enemyEntity, _builder->Mesh()->GetMesh(_enemyEntity));
	_builder->GetEntityController()->Transform()->MoveForward(_enemyEntity, -1);
}

Enemy::~Enemy()
{
	if (_myPath)
	{
		delete[] _myPath->nodes;
		_myPath->nodes = nullptr;
		delete _myPath;
		_myPath = nullptr;
	}

	_builder->GetEntityController()->ReleaseEntity(_enemyEntity);
}

Entity Enemy::GetEntity()
{
	return _enemyEntity;
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
	if (_myPath)
	{
		delete _myPath->nodes;
		delete _myPath;
		_myPath = nullptr;
	}

	float lengthToPlayer = XMVectorGetX(XMVector3Length(XMLoadFloat3(&GetCurrentPos()) - playerPosition));
	if (lengthToPlayer > 1.5f)
	{
		XMVECTOR move = XMLoadFloat3(&_movementVector);
		move = XMVector3Normalize(playerPosition - _builder->Transform()->GetPosition(_enemyEntity));
		XMStoreFloat3(&_movementVector, move);

		_builder->Transform()->MoveAlongVector(_enemyEntity, move*deltaTime);
	}



}

void Enemy::GivePath(Path* newPath)
{
	if (_myPath)
	{
		delete _myPath->nodes;
		delete _myPath;
	}
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
	//"Allign it" to the Node system.
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