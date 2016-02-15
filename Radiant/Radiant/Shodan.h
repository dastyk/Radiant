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
	VeryBasicAI* _pathfinding;
	EntityBuilder* _builder;
	MapNode** _dungeon;
	int _sizeOfDungeonSide;
	List<Enemy> _Entities;
	int* _walkableNodes;
	int _nrOfWalkableNodesAvailable;

	Shodan();

public:
	Shodan(EntityBuilder* builder, Dungeon* dungeon, int sizeOfSide);
	~Shodan();

	void Update(float deltaTime);
	void AddEnemy();
};











#endif