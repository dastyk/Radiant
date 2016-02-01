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

const void BoundingManager::_TransformChanged(const Entity & entity, const DirectX::XMMATRIX & world)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		DirectX::XMStoreFloat4x4(&_data[indexIt->second].world, world);
	}
	return void();
}