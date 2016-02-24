#ifndef _BOUNDINGMANAGER_H_
#define _BOUNDINGMANAGER_H_

#pragma once
#include "TransformManager.h"
#include "Entity.h"
#include "Mesh.h"
#include "Collision.h"
#include "Utils.h"

struct AABBTD
{
	AABBT lT;
	AABBT tT;
};
struct BSD
{
	DirectX::BoundingSphere lBS;
	DirectX::BoundingSphere tBS;
};
struct AABBD
{
	DirectX::BoundingBox lAABB;
	DirectX::BoundingBox tAABB;
};
class BoundingManager
{
public:
	BoundingManager(TransformManager& trans);
	~BoundingManager();


	const void CreateQuadTree(const Entity& entity, const std::vector<Entity>& entities);
	const void CreateBoundingBox(const Entity& entity, const Mesh* mesh);
	const void CreateBoundingBox(const Entity& entity, float width, float height, float depth);
	const void CreateBoundingSphere(const Entity& entity, float radius);
	const void CreateBoundingSphere(const Entity& entity, const Mesh* mesh);
	const bool CheckCollision(const Entity& entity, const Entity& entity2)const;
	const bool GetMTV(const Entity& entity, const Entity& entity2, std::function<void(DirectX::XMVECTOR& outMTV)> move)const;


	const void GetEntitiesInFrustum(const DirectX::BoundingFrustum& frustum, std::vector<Entity>& entites);
	const void GetEntitiesInFrustumNoQuadTree(const DirectX::BoundingFrustum& frustum, std::vector<Entity>& entites);


	const void ReleaseBoundingData(const Entity& entity);

private:
	void _TransformChanged( const Entity& entity, const DirectX::XMMATRIX& tran, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& up );

private:
	std::unordered_map<Entity, QuadTree*, EntityHasher> _entityToQuadTree;
	std::unordered_map<Entity, BSD*, EntityHasher> _entityToBS;
	std::unordered_map<Entity, AABBD*, EntityHasher> _entityToAABB;
	Collision* _collision;
};
#endif
