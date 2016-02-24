#ifndef _SHODAN_H
#define _SHODAN_H

#include "VeryBasicAI.h"
#include "EntityBuilder.h"
#include "Dungeon.h"
#include "Enemy.h"
#include "List.h"
#include "AI_Defines.h"
#include "AIStateController.h"
#include "Projectile.h"
#include "Player.h"

#define STARTRANGELIGHT 5.0f

class Shodan
{
private:
	struct EnemyWithStates
	{
		Enemy* _thisEnemy;
		AIStateController* _thisEnemyStateController;
		~EnemyWithStates() { delete _thisEnemy; delete _thisEnemyStateController; };
	};

	VeryBasicAI* _pathfinding = nullptr;
	EntityBuilder* _builder = nullptr;
	MapNode** _dungeon = nullptr;
	int _sizeOfDungeonSide;
	List<EnemyWithStates> _Entities;
	int* _walkableNodes = nullptr;
	int _nrOfWalkableNodesAvailable;
	bool _playerSeen = false;
	float _timeUntilWeCheckForPlayer;
	XMVECTOR _playerSeenAt;
	XMVECTOR _playerCurrentPosition;
	Player* _playerPointer;

	int _nrOfStartingEnemies;
	float _lightPoolPercent;

	Shodan();

public:
	Shodan(EntityBuilder* builder, Dungeon* dungeon, int sizeOfSide, Player* thePlayer);
	~Shodan();

	void Update(float deltaTime, DirectX::XMVECTOR playerPosition);
	void ChangeLightLevel(float lightLevel);
	void AddEnemy();

	float GetLightPoolPercent();

	void CheckCollisionAgainstProjectiles(vector<Projectile*> projectiles);
	void CheckIfPlayerIsHit(vector<Projectile*> projectiles);
	Path* NeedPath(Entity entityToGivePath);
	Path* NeedPath(Entity entityToGivePath, XMFLOAT3 goal);
	bool PlayerSeen();
	bool CheckIfPlayerIsSeenForEnemy(Enemy* enemyToCheck);
	XMVECTOR PlayerCurrentPosition();
	bool NodeWalkable(float x, float y);

};











#endif