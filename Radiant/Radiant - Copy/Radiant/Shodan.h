#ifndef _SHODAN_H
#define _SHODAN_H

#include "VeryBasicAI.h"
#include "Dungeon.h"
#include "List.h"
#include "AI_Defines.h"
#include "Projectile.h"
#include "Player.h"
#include "EnemyBuilder.h"

class Shodan
{
private:

	VeryBasicAI* _pathfinding = nullptr;
	EntityBuilder* _builder = nullptr;
	MapNode** _dungeon = nullptr;
	int _sizeOfDungeonSide;
	EnemyBuilder* _enemyBuilder;
	List<EnemyWithStates> _Entities;
	vector<Projectile*> _enemyProjectiles;
	vector<Projectile*> _playerFriendlyProjectiles;
	int* _walkableNodes = nullptr;
	int _nrOfWalkableNodesAvailable;
	bool _playerSeen = false;
	float _timeUntilWeCheckForPlayer;
	XMVECTOR _playerSeenAt;
	XMVECTOR _playerCurrentPosition;
	Player* _playerPointer;
	float _timeSincePlayerHitSound;

	int _nrOfStartingEnemies;
	float _lightPoolPercent;

	Shodan();

	void _CheckIfPlayerIsHit(float deltaTime);
	void _AddEnemyFromListOfPositions(int *nodesToTakeFrom, int nrOfNodes);

public:
	Shodan(EntityBuilder* builder, Dungeon* dungeon, int sizeOfSide, Player* thePlayer);
	~Shodan();

	void Update(float deltaTime, DirectX::XMVECTOR playerPosition);
	void ChangeLightLevel(float lightLevel);
	void AddEnemy();

	float GetLightPoolPercent();

	void CheckCollisionAgainstProjectiles(const vector<Projectile*>& projectiles);
	Path* NeedPath(Entity entityToGivePath);
	Path* NeedPath(Entity entityToGivePath, XMFLOAT3 goal);
	bool PlayerSeen();
	bool CheckIfPlayerIsSeenForEnemy(Enemy* enemyToCheck);
	XMVECTOR PlayerCurrentPosition();
	bool NodeWalkable(float x, float y);
	Enemy* GetClosestEnemy(Entity thisEntity);
	void AddPlayerFriendlyProjectiles(Enemy *thisEnemy);
	void AddEnemyAroundPoint(XMFLOAT3 pointToRandomAround, int range);

	List<EnemyWithStates>* GetEnemyList();

};











#endif