#ifndef _BOUNDINGMANAGER_H_
#define _BOUNDINGMANAGER_H_

#pragma once
#include "TransformManager.h"
#include "Entity.h"
#include "Mesh.h"
#include "Collision.h"
#include "Utils.h"

struct BoundingData
{
	BBT bbt;
	DirectX::BoundingOrientedBox obb;
	BBT testAgainstBBT;
};

class BoundingManager
{
public:
	BoundingManager(TransformManager& trans);
	~BoundingManager();


	const void CreateBoundingBox(const Entity& entity, const Mesh* mesh);
	const bool CheckCollision(const Entity& entity, const Entity& entity2)const;

	const void ReleaseBoundingData(const Entity& entity);

private:
	void _TransformChanged( const Entity& entity, const DirectX::XMMATRIX& tran, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& up );

private:
	std::vector<BoundingData> _data;

	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
	std::unordered_map<Entity, BoundingData, EntityHasher> _entityToBoundingData;
	Collision* _collision;
};
#endif
