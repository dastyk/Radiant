#include "BoundingManager.h"

using namespace DirectX;

BoundingManager::BoundingManager(TransformManager& trans)
{
	_collision = nullptr;
	try { _collision = new Collision(); }
	catch (std::exception& e) { e; throw ErrorMsg(1300001, L"Failed to create collision instance"); }

	trans.TransformChanged += Delegate<void( const Entity&, const XMMATRIX&, const XMVECTOR&, const XMVECTOR&, const XMVECTOR& )>::Make<BoundingManager, &BoundingManager::_TransformChanged>( this );
}


BoundingManager::~BoundingManager()
{
	/*for (auto& bbt : _data)
	{
		bbt.bbt.Release();
		bbt.testAgainstBBT.Release();
	}
	SAFE_DELETE(_collision);*/
	for (auto &bd : _entityToBoundingData)
	{
		bd.second.bbt.Release();
		bd.second.testAgainstBBT.Release();
	}
	SAFE_DELETE(_collision);
}

const void BoundingManager::CreateBoundingBox(const Entity& entity, const Mesh* mesh)
{
	//auto indexIt = _entityToIndex.find(entity);

	//if (indexIt != _entityToIndex.end())
	//{
	//	TraceDebug("Tried to bind bounding component to entity that already had one.");
	//	return;
	//}

	//const std::vector<float>& pos = mesh->AttributeData(mesh->FindStream(Mesh::AttributeType::Position));
	//const uint * in = mesh->AttributeIndices(mesh->FindStream(Mesh::AttributeType::Position));
	//const std::vector<Mesh::Batch>& b = mesh->Batches();
	//
	//BoundingData data;
	//data.bbt = _collision->CreateBBT((DirectX::XMFLOAT3*)&pos[0], sizeof(DirectX::XMFLOAT3), (uint*)in, (SubMeshInfo*)&b[0], static_cast<unsigned int>(b.size()));
	//
	//_entityToIndex[entity] = static_cast<int>(_data.size());
	//_data.push_back(std::move(data));

	//return void();
	auto got = _entityToBoundingData.find(entity);
	if (got != _entityToBoundingData.end())
	{
		TraceDebug("Tried to bind bounding compononet to entity that already had one");
		return;
	}
	const std::vector<float>& pos = mesh->AttributeData(mesh->FindStream(Mesh::AttributeType::Position));
	const uint * in = mesh->AttributeIndices(mesh->FindStream(Mesh::AttributeType::Position));
	const std::vector<Mesh::Batch>& b = mesh->Batches();

	BoundingData data;
	data.bbt =_collision->CreateBBT((DirectX::XMFLOAT3*)&pos[0], sizeof(DirectX::XMFLOAT3), (uint*)in, (SubMeshInfo*)&b[0], static_cast<unsigned int>(b.size()));
	_entityToBoundingData[entity] = std::move(data);
	
	return void();
}

const bool BoundingManager::CheckCollision(const Entity & entity, const Entity & entity2) const
{
	//auto indexIt = _entityToIndex.find(entity);

	//if (indexIt != _entityToIndex.end())
	//{
	//	auto i2 = _entityToIndex.find(entity2);
	//	if (i2 != _entityToIndex.end())
	//	{
	//		//int test = _collision->CheckSingleAgainstSingle(_data[indexIt->second].obb, _data[i2->second].obb);

	//		int test = _collision->TestBBTAgainstBBT(_data[indexIt->second].testAgainstBBT, _data[i2->second].testAgainstBBT);

	//		if (test != 0)
	//			return true;
	//		else
	//			return false;
	//	}

	//}

	//TraceDebug("Tried to check collision for a entity with no bounding box.");
	//return false;
	auto got = _entityToBoundingData.find(entity);

	if (got != _entityToBoundingData.end())
	{
		auto got2 = _entityToBoundingData.find(entity2);
		if (got2 != _entityToBoundingData.end())
		{
			int test = _collision->TestBBTAgainstBBT(got->second.testAgainstBBT, got2->second.testAgainstBBT);
			return test != 0;
		}
	}
	TraceDebug("Tried to check collision for an entity with no bounding box");
	return false;
}

const void BoundingManager::ReleaseBoundingData(const Entity & entity)
{
	auto got = _entityToBoundingData.find(entity);
	if (got != _entityToBoundingData.end())
	{
		_entityToBoundingData.erase(got);
	}
	return void();
}

void BoundingManager::_TransformChanged( const Entity& entity, const XMMATRIX& tran, const XMVECTOR& pos, const XMVECTOR& dir, const XMVECTOR& up )
{
	//auto indexIt = _entityToIndex.find(entity);

	//if (indexIt != _entityToIndex.end())
	//{
	//	_data[indexIt->second].testAgainstBBT = _collision->TransformBBT(_data[indexIt->second].bbt, world);
	//	//_data[indexIt->second].bbt.root.Transform(_data[indexIt->second].obb, world);

	//	//DirectX::XMStoreFloat4x4(&_data[indexIt->second].world, world);
	//}
	//return void();
	auto got = _entityToBoundingData.find(entity);
	if (got != _entityToBoundingData.end())
	{
		got->second.testAgainstBBT = _collision->TransformBBT(got->second.bbt, tran);
	}
	return void();
}
