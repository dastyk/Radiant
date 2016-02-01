#include "BoundingManager.h"



BoundingManager::BoundingManager(TransformManager& trans)
{
	_collision = nullptr;
	try { _collision = new Collision(); }
	catch (std::exception& e) { e; throw ErrorMsg(1300001, L"Failed to create collision instance"); }

	trans.SetTransformChangeCallback6([this](const Entity& entity, const DirectX::XMMATRIX & world)
	{
		_TransformChanged(entity, world);
	});
}


BoundingManager::~BoundingManager()
{
	SAFE_DELETE(_collision);
}

const void BoundingManager::CreateBoundingBox(const Entity& entity, const Mesh* mesh)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		TraceDebug("Tried to bind bounding component to entity that already had one.");
		return;
	}

	const std::vector<float>& pos = mesh->AttributeData(mesh->FindStream(Mesh::AttributeType::Position));
	const uint * in = mesh->AttributeIndices(mesh->FindStream(Mesh::AttributeType::Position));
	const std::vector<Mesh::Batch>& b = mesh->Batches();
	
	BoundingData data;
	data.bbt = _collision->CreateBBT((DirectX::XMFLOAT3*)&pos[0], sizeof(DirectX::XMFLOAT3), (uint*)in, (SubMeshInfo*)&b[0], b.size());
	DirectX::XMStoreFloat4x4(&data.world, DirectX::XMMatrixIdentity());
	
	
	_entityToIndex[entity] = static_cast<int>(_data.size());
	_data.push_back(std::move(data));

	return void();
}

const bool BoundingManager::CheckCollision(const Entity & entity, const Entity & entity2) const
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		auto i2 = _entityToIndex.find(entity2);
		if (i2 != _entityToIndex.end())
		{
			DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&_data[i2->second].world);
			DirectX::BoundingOrientedBox b;
			_data[i2->second].bbt.root.Transform(b, mat);
			int test = _collision->TestBBTAgainstSingle(_data[indexIt->second].bbt, b);
			if (test != 0)
				return true;
			else
				return false;
		}

	}

	TraceDebug("Tried to check collision for a entity with no bounding box.");
	return false;
}

const void BoundingManager::_TransformChanged(const Entity & entity, const DirectX::XMMATRIX & world)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		if (entity.ID != 0)
			int i = 0;
		DirectX::XMStoreFloat4x4(&_data[indexIt->second].world, world);
	}
	return void();
}
