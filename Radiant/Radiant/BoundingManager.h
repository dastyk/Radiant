#ifndef _BOUNDINGMANAGER_H_
#define _BOUNDINGMANAGER_H_

#pragma once
#include "TransformManager.h"
#include "Entity.h"
#include "Mesh.h"

struct BoundingData
{

};

class BoundingManager
{
public:
	BoundingManager(TransformManager& trans);
	~BoundingManager();


	const void CreateBoundingBox(Entity& entity, Mesh* mesh);


private:
	const void _TransformChanged(const Entity& entity, const DirectX::XMMATRIX& pos);
private:
	std::vector<BoundingData> _cOverlays;

	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
};
#endif
