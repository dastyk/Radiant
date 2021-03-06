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
	SAFE_DELETE(_collision);

	std::vector<Entity> v;
	for (auto& b : _entityToQuadTree)
		v.push_back(b.first);

	for (auto& b : _entityToBS)
		v.push_back(b.first);

	for (auto& b : _entityToAABB)
		v.push_back(b.first);

	for (auto& b : v)
		ReleaseBoundingData(b);
	

}

const void BoundingManager::CreateQuadTree(const Entity & entity, const std::vector<Entity>& entities)
{
	auto indexIt = _entityToQuadTree.find(entity);

	if (indexIt != _entityToQuadTree.end())
	{
		SAFE_DELETE(indexIt->second);
	}
	std::vector<DirectX::BoundingBox> boxes;
	std::vector<Entity> ents;

	for (auto& e : entities)
	{
		auto got = _entityToAABB.find(e);
		if (got != _entityToAABB.end())
		{
			boxes.push_back(got->second->tAABB);
			ents.push_back(e);
		}
	}
	for (auto& e : ents)
		ReleaseBoundingData(e);

	QuadTree * tree = new QuadTree(entities, boxes);

	_entityToQuadTree[entity] = tree;
	return void();
}

const void BoundingManager::CreateBoundingBox(const Entity& entity, const Mesh* mesh)
{
	auto indexIt = _entityToAABB.find(entity);

	if (indexIt != _entityToAABB.end())
	{
		SAFE_DELETE(indexIt->second);
	}
	const std::vector<float>& pos = mesh->AttributeData(mesh->FindStream(Mesh::AttributeType::Position));

	AABBD * data = new AABBD;
	data->lAABB.CreateFromPoints(data->lAABB, pos.size()/3, (DirectX::XMFLOAT3*)&pos[0], sizeof(DirectX::XMFLOAT3));
	data->tAABB = data->lAABB;

	_entityToAABB[entity] = data;
	return void();
}

const void BoundingManager::CreateBoundingBox(const Entity& entity, float width, float height, float depth)
{
	auto indexIt = _entityToAABB.find(entity);

	if (indexIt != _entityToAABB.end())
	{
		SAFE_DELETE(indexIt->second);
	}
	AABBD * data = new AABBD;
	data->lAABB.Center.x = 0.0f;
	data->lAABB.Center.y = 0.0f;
	data->lAABB.Center.z = 0.0f;
	data->lAABB.Extents.x = width;
	data->lAABB.Extents.y = height;
	data->lAABB.Extents.z = depth;

	data->tAABB = data->lAABB;
	_entityToAABB[entity] = data;

	return void();
}

const void BoundingManager::CreateBoundingSphere(const Entity & entity, float radius)
{
	auto indexIt = _entityToBS.find(entity);

	if (indexIt != _entityToBS.end())
	{
		SAFE_DELETE(indexIt->second);
	}
	BSD * data = new BSD;
	data->lBS.Center.x = 0.0f;
	data->lBS.Center.y = 0.0f;
	data->lBS.Center.z = 0.0f;
	data->lBS.Radius = radius;

	data->tBS = data->lBS;
	_entityToBS[entity] = data;

	return void();
}

const void BoundingManager::CreateBoundingSphere(const Entity & entity, const Mesh * mesh)
{
	auto indexIt = _entityToBS.find(entity);

	if (indexIt != _entityToBS.end())
	{
		SAFE_DELETE(indexIt->second);
	}

	const std::vector<float>& pos = mesh->AttributeData(mesh->FindStream(Mesh::AttributeType::Position));

	BSD * data = new BSD;
	data->lBS.CreateFromPoints(data->lBS, pos.size() / 3, (DirectX::XMFLOAT3*)&pos[0], sizeof(DirectX::XMFLOAT3));
	data->tBS = data->lBS;
	_entityToBS[entity] = data;

	return void();
}

const bool BoundingManager::CheckCollision(const Entity & entity, const Entity & entity2) const
{

	auto goto2 = _entityToBS.find(entity2);
	auto goto3 = _entityToAABB.find(entity2);


	auto gote2 = _entityToBS.find(entity);


	if (gote2 != _entityToBS.end())
	{


		if (goto2 != _entityToBS.end())
		{
			int test = _collision->CheckSingleAgainstSingle(gote2->second->tBS, goto2->second->tBS);
			return test != 0;
		}


		if (goto3 != _entityToAABB.end())
		{
			int test = _collision->CheckSingleAgainstSingle(gote2->second->tBS, goto3->second->tAABB);
			return test != 0;
		}
	}

	auto gote3 = _entityToAABB.find(entity);


	if (gote3 != _entityToAABB.end())
	{



		if (goto2 != _entityToBS.end())
		{
			int test = _collision->CheckSingleAgainstSingle(gote3->second->tAABB, goto2->second->tBS);
			return test != 0;
		}


		if (goto3 != _entityToAABB.end())
		{
			int test = _collision->CheckSingleAgainstSingle(gote3->second->tAABB, goto3->second->tAABB);
			return test != 0;
		}
	}



	return false;
}

const bool BoundingManager::GetMTV(const Entity & entity, const Entity & entity2, std::function<void(DirectX::XMVECTOR& outMTV, const Entity& entity)> move) const
{
	auto gote1 = _entityToQuadTree.find(entity);
	auto goto1 = _entityToBS.find(entity);

	auto gote2 = _entityToQuadTree.find(entity2);
	auto goto2 = _entityToBS.find(entity2);

	if (gote1 != _entityToQuadTree.end())
	{
	
		if (goto2 != _entityToBS.end())
		{
			return gote1->second->GetMTV(goto2->second->tBS, move);
		}
	}

	if (gote2 != _entityToQuadTree.end())
	{

		if (goto1 != _entityToBS.end())
		{
			return gote2->second->GetMTV(goto1->second->tBS, move);
		}
	}


	/*auto gote2 = _entityToAABB.find(entity);
	if (gote2 != _entityToAABB.end())
	{
		auto goto2 = _entityToBS.find(entity2);
		if (goto2 != _entityToBS.end())
		{
			return _collision->CheckSingleAgainstSingle(gote2->second->tAABB, goto2->second->tBS, outMTV);
		}
	}*/

	return false;
}

const void BoundingManager::GetEntitiesInFrustum(const DirectX::BoundingFrustum & frustum, std::vector<Entity>& entites)
{
	for (auto& tree : _entityToQuadTree)
	{
		tree.second->GetEntitiesInFrustum(frustum, entites);
	}
}

const void BoundingManager::GetEntitiesInFrustumNoQuadTree(const DirectX::BoundingFrustum & frustum, std::vector<Entity>& entites)
{

	for (auto& b : _entityToBS)
	{
		int test = _collision->CheckSingleAgainstSingle(frustum, b.second->tBS);
		if (test != 0)
		{
			entites.push_back(b.first);
		}
	}

	for (auto& b : _entityToAABB)
	{
		int test = _collision->CheckSingleAgainstSingle(frustum, b.second->tAABB);
		if (test != 0)
		{
			entites.push_back(b.first);
		}
	}
}

const void BoundingManager::ReleaseBoundingData(const Entity & entity)
{
	auto got = _entityToBS.find(entity);
	if (got != _entityToBS.end())
	{
		SAFE_DELETE(got->second);
		_entityToBS.erase(got->first);
	}
	auto got2 = _entityToAABB.find(entity);
	if (got2 != _entityToAABB.end())
	{
		SAFE_DELETE(got2->second);
		_entityToAABB.erase(got2->first);
	}
	auto got3 = _entityToQuadTree.find(entity);
	if (got3 != _entityToQuadTree.end())
	{
		SAFE_DELETE(got3->second);
		_entityToQuadTree.erase(got3->first);
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

	auto got2 = _entityToBS.find(entity);
	if (got2 != _entityToBS.end())
	{
		got2->second->lBS.Transform(got2->second->tBS, tran);
	}
	auto got3 = _entityToAABB.find(entity);
	if (got3 != _entityToAABB.end())
	{
		got3->second->lAABB.Transform(got3->second->tAABB, tran);
	}
	return void();
}
