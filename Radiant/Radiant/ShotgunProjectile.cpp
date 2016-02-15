#include "ShotgunProjectile.h"
#include "System.h"

ShotgunProjectile::ShotgunProjectile(Entity playerEntity, EntityBuilder* builder) : Projectile(builder)
{
	_lifeTime = 1.0f;
	_alive = true;
	_damage = 30.0f;

	// Create dummy model for collision -- Needs to be changed if starting camera position changes -- Also should probably look at near plane position and adjust to avoid clipping

	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT2> uv;
	std::vector<unsigned int> indices;

	positions.resize(16);
	uv.resize(16);
	indices.resize(24);

	float extent = 0.15;

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

	_projectileEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_projectileEntity);
	_builder->Mesh()->CreateStaticMesh(_projectileEntity, "Projectile", positions, uv, indices);
	_builder->Mesh()->ToggleVisibility(_projectileEntity, 0);
	_builder->Material()->BindMaterial(_projectileEntity, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_projectileEntity, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_projectileEntity, 0, "Metalic", 0.1f, "Shaders/GBuffer.hlsl");
	_builder->Bounding()->CreateBoundingBox(_projectileEntity, _builder->Mesh()->GetMesh(_projectileEntity));
	_builder->Light()->BindPointLight(_projectileEntity, XMFLOAT3(0, 0, 0), 1, XMFLOAT3(0.0f, 1.0f, 0.0f), 5);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, _builder->Transform()->GetPosition(playerEntity));
	_builder->GetEntityController()->Transform()->SetPosition(_projectileEntity, temp);
	XMStoreFloat3(&temp, _builder->Transform()->GetRotation(playerEntity));
	_builder->GetEntityController()->Transform()->SetRotation(_projectileEntity, temp);
	_builder->Transform()->MoveForward(_projectileEntity, 0);

	for (int i = 0; i < NROFSPREAD; i++)
		_spread[i] = new ShotgunProjectile(_builder->Transform()->GetPosition(playerEntity), _builder->Transform()->GetRotation(playerEntity), _builder);

}

ShotgunProjectile::ShotgunProjectile(XMVECTOR originPos, XMVECTOR originDir, EntityBuilder * builder) : Projectile(builder)
{
	_lifeTime = 1.0f;
	_alive = true;
	_damage = 30.0f;

	// Create dummy model for collision -- Needs to be changed if starting camera position changes -- Also should probably look at near plane position and adjust to avoid clipping

	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT2> uv;
	std::vector<unsigned int> indices;

	positions.resize(16);
	uv.resize(16);
	indices.resize(24);

	float extent = 0.15;

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

	_projectileEntity = _builder->EntityC().Create();
	_builder->Transform()->CreateTransform(_projectileEntity);
	_builder->Mesh()->CreateStaticMesh(_projectileEntity, "Projectile", positions, uv, indices);
	_builder->Mesh()->ToggleVisibility(_projectileEntity, 0);
	_builder->Material()->BindMaterial(_projectileEntity, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_projectileEntity, 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(_projectileEntity, 0, "Metalic", 0.1f, "Shaders/GBuffer.hlsl");
	_builder->Bounding()->CreateBoundingBox(_projectileEntity, _builder->Mesh()->GetMesh(_projectileEntity));
	_builder->Light()->BindPointLight(_projectileEntity, XMFLOAT3(0, 0, 0), 1, XMFLOAT3(0.0f, 1.0f, 0.0f), 5);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, originPos);
	_builder->GetEntityController()->Transform()->SetPosition(_projectileEntity, temp);
	XMStoreFloat3(&temp, originDir);
	temp.x += (rand() % 4001 - 2000) / 100.0;
	temp.y += (rand() % 4001 - 2000) / 100.0;
	temp.z += (rand() % 4001 - 2000) / 100.0;
	_builder->GetEntityController()->Transform()->SetRotation(_projectileEntity, temp);
	_builder->Transform()->MoveForward(_projectileEntity, 0);

	for (int i = 0; i < NROFSPREAD; i++)
		_spread[i] = nullptr;

}

ShotgunProjectile::~ShotgunProjectile()
{
	for (int i = 0; i < NROFSPREAD; i++)
		SAFE_DELETE(_spread[i]);
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
	}

	for (int i = 0; i < NROFSPREAD; i++)
	{
		if(_spread[i] != nullptr)
			_spread[i]->Update(deltaTime);
	}
}

bool ShotgunProjectile::GetState()
{
	return _alive;
}