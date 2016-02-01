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
	DirectX::XMFLOAT4X4 world;
};

class BoundingManager
{
public:
	BoundingManager(TransformManager& trans);
	~BoundingManager();


	const void CreateBoundingBox(const Entity& entity, const Mesh* mesh);
	//const bool CheckCollision(const Entity& entity, const Entity& entity2)const;

private:
	const void _TransformChanged(const Entity& entity, const DirectX::XMMATRIX& pos);
private:
	std::vector<BoundingData> _data;

	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;

	Collision* _collision;
};
#endif