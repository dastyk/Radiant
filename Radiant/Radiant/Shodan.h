#ifndef _SHODAN_H
#define _SHODAN_H
#include "VeryBasicAI.h"
#include "EntityBuilder.h"
#include "Dungeon.h"
#include "Enemy.h"
#include "List.h"

class Shodan
{
private:
	VeryBasicAI* _pathfinding = nullptr;
	EntityBuilder* _builder = nullptr;
	MapNode** _dungeon = nullptr;
	int _sizeOfDungeonSide;
	List<Enemy> _Entities;
	int* _walkableNodes = nullptr;
	int _nrOfWalkableNodesAvailable;

	Shodan();

public:
	Shodan(EntityBuilder* builder, Dungeon* dungeon, int sizeOfSide);
	~Shodan();

	void Update(float deltaTime, DirectX::XMVECTOR playerPosition);
	void ChangeLightLevel(float lightLevel);
	void AddEnemy();
};











#endif