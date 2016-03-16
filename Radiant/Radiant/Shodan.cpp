#include "Shodan.h"
#include "System.h"

#define LengthForUpdate 0.707
using namespace DirectX;

Shodan::Shodan()
{

}

Shodan::Shodan(EntityBuilder* builder, Dungeon* map, int sizeOfSide, Player* thePlayer) : _builder(builder)
{
	_sizeOfDungeonSide = sizeOfSide;
	_dungeon = new MapNode*[sizeOfSide*sizeOfSide*4];
	for (int i = 0; i < sizeOfSide*sizeOfSide * 4; i++)
	{
		_dungeon[i] = nullptr;
	}
	_walkableNodes = new int[sizeOfSide*sizeOfSide*4];
	ZeroMemory(_dungeon, sizeOfSide*sizeOfSide * 4 * sizeof(int));
	_nrOfWalkableNodesAvailable = 0;
	_playerPointer = thePlayer;
	XMVECTOR p = _builder->Transform()->GetPosition(_playerPointer->GetEntity());
	XMStoreFloat3(&_playerCurrentPosition, p);
	_enemyBuilder = new EnemyBuilder(_builder, this);
	_timeSincePlayerHitSound = 0.0f;

	MapGridPairs giveMe;
	giveMe.x = 0.0f;	
	giveMe.y = 0.0f;
	//Divide each cell of the dungeon into a 2x2 grid.

	//============
	//== 1 == 2 ==
	//== 3 == 4 ==
	//============

	int j = 0;
	int countForVector = 0;
	auto &vectorOfPositions = map->GetFreePositions();
	for (int i = 0; i < (sizeOfSide)*(sizeOfSide); i++)
	{
		MapNode* node1 = new MapNode();
		ZeroMemory(node1, sizeof(MapNode));
		node1->ID = j;
		node1->position = giveMe;
		node1->parentMapNode = j;
		node1->type = 1;
		_dungeon[j] = node1;


		MapNode* node2 = new MapNode(*node1);
		ZeroMemory(node2, sizeof(MapNode));
		node2->position = MapGridPairs(giveMe.x + 0.50f, giveMe.y);
		node2->ID = j + 1;
		node2->parentMapNode = j + 1;
		_dungeon[j + 1] = node2;


		MapNode* node3 = new MapNode(*node1);
		ZeroMemory(node3, sizeof(MapNode));
		node3->position = MapGridPairs(giveMe.x, giveMe.y + 0.50f);
		node3->ID = j + sizeOfSide * 2;
		node3->parentMapNode = j + sizeOfSide * 2;
		_dungeon[j + sizeOfSide*2] = node3;


		MapNode* node4 = new MapNode(*node1);
		ZeroMemory(node4, sizeof(MapNode));
		node4->position = MapGridPairs(giveMe.x + 0.50f, giveMe.y + 0.50f);
		node4->ID = j + sizeOfSide * 2 + 1;
		node4->parentMapNode = j + sizeOfSide * 2 + 1;
		_dungeon[j + sizeOfSide * 2 + 1] = node4;



		for (auto &thisPosition : vectorOfPositions)
		{
			if (thisPosition.x == giveMe.x && thisPosition.y == giveMe.y)
			{
				node1->type = 0;
				node2->type = 0;
				node3->type = 0;
				node4->type = 0;
				_walkableNodes[_nrOfWalkableNodesAvailable] = node1->ID;
				_walkableNodes[_nrOfWalkableNodesAvailable + 1] = node2->ID;
				_walkableNodes[_nrOfWalkableNodesAvailable + 2] = node3->ID;
				_walkableNodes[_nrOfWalkableNodesAvailable + 3] = node4->ID;
				_nrOfWalkableNodesAvailable += 4;
				countForVector++;
			}
		}
		

		j += 2;

		if (j % (sizeOfSide * 2) == 0)
		{
			j += sizeOfSide*2;
		}

		giveMe.x++;
		if (!(int(giveMe.x) % (sizeOfSide)))
		{
			giveMe.y++;
			giveMe.x = 0.0f;
		}
	}
	

	_sizeOfDungeonSide = sizeOfSide * 2;

	_pathfinding = new VeryBasicAI(_dungeon, _sizeOfDungeonSide*_sizeOfDungeonSide);


	_timeUntilWeCheckForPlayer = 2.0f;
	_playerSeen = false;
	_playerSeenAt = XMFLOAT3(0.0f, 0.0f, 0.0f);

	_lightPoolPercent = 1.0f;

}

Shodan::~Shodan()
{
	SAFE_DELETE(_pathfinding);
	for (int i = 0; i < _sizeOfDungeonSide*_sizeOfDungeonSide; i++)
	{
		delete _dungeon[i];
	}
	delete[] _dungeon;
	SAFE_DELETE(_walkableNodes);
	while (_Entities.Size())
	{
		_Entities.RemoveCurrentElement();
	}
	SAFE_DELETE(_enemyBuilder);
	for (int i = 0; i < _enemyProjectiles.size(); i++)
	{
		delete _enemyProjectiles[i];
	}
	for (int i = 0; i < _playerFriendlyProjectiles.size(); i++)
	{
		delete _playerFriendlyProjectiles[i];
}
}

void Shodan::Update(float deltaTime, const DirectX::XMVECTOR& playerPosition)
{
	XMStoreFloat3(&_playerCurrentPosition, playerPosition);

	for (int i = 0; i < _Entities.Size(); i++)
	{
		Enemy* tempEnemy = _Entities.GetCurrentElement()->_thisEnemy;
		float lengthToPlayer = XMVectorGetX(XMVector3Length(XMLoadFloat3(&tempEnemy->GetCurrentPos()) - playerPosition));
		if (lengthToPlayer < _sizeOfDungeonSide*LengthForUpdate)
		{
			if (tempEnemy->GetWeapon() != nullptr)
			{
			vector<Projectile*> temp = _Entities.GetCurrentElement()->_thisEnemy->GetWeapon()->GetProjectilesOwnership();
			if (temp.size())
			{
				_enemyProjectiles.insert(_enemyProjectiles.end(), temp.begin(), temp.end());
			}
			}
			_Entities.GetCurrentElement()->_thisEnemyStateController->UpdateMachine(deltaTime);

		}
		_Entities.MoveCurrent();
	}
	if(_Entities.Size())
	if (_Entities.GetCurrentElement()->_thisEnemy->GetCurrentStatusEffects() == STATUS_EFFECT_TIME_STOP)
		return;

	for (int i = 0; i < _enemyProjectiles.size(); i++)
	{
		XMVECTOR temp = _builder->Transform()->GetPosition(_enemyProjectiles[i]->GetEntity());
		float yPosition = XMVectorGetY(temp);
		float lightRange = _builder->Light()->GetLightBlobRange(_enemyProjectiles[i]->GetEntity());
		if (!NodeWalkable(XMVectorGetX(temp), XMVectorGetZ(temp)) || yPosition < 0.0f || yPosition > 3.5f)
			_enemyProjectiles[i]->SetState(false);

		if (!_enemyProjectiles[i]->GetState())
		{
			delete _enemyProjectiles[i];
			_enemyProjectiles.erase(_enemyProjectiles.begin() + i);
			_enemyProjectiles.shrink_to_fit();
			i--;
		}
	}
	for (int i = 0; i < _playerFriendlyProjectiles.size(); i++)
	{
		_playerFriendlyProjectiles[i]->Update(deltaTime);
	}
	for (int i = 0; i < _playerFriendlyProjectiles.size(); i++)
	{
		XMVECTOR temp = _builder->Transform()->GetPosition(_playerFriendlyProjectiles[i]->GetEntity());
		float yPosition = XMVectorGetY(temp);
		float lightRange = _builder->Light()->GetLightBlobRange(_playerFriendlyProjectiles[i]->GetEntity());
		if (!NodeWalkable(XMVectorGetX(temp), XMVectorGetZ(temp)) || yPosition < 0.0f || yPosition > 3.5f)
			_playerFriendlyProjectiles[i]->SetState(false);

		if (!_playerFriendlyProjectiles[i]->GetState())
		{
			delete _playerFriendlyProjectiles[i];
			_playerFriendlyProjectiles.erase(_playerFriendlyProjectiles.begin() + i);
			_playerFriendlyProjectiles.shrink_to_fit();
			i--;
		}
	}
	_CheckIfPlayerIsHit(deltaTime);
}

void Shodan::AddPlayerFriendlyProjectiles(Enemy *thisEnemy)
{
	vector<Projectile*> temp = thisEnemy->GetWeapon()->GetProjectilesOwnership();
	if (temp.size())
	{
		_playerFriendlyProjectiles.insert(_playerFriendlyProjectiles.end(), temp.begin(), temp.end());
	}
}

bool Shodan::CheckIfPlayerIsSeenForEnemy(Enemy* enemyToCheck)
{
	XMVECTOR playerPos = XMLoadFloat3(&_playerCurrentPosition);
	float lengthToPlayer = XMVectorGetX(XMVector3Length(XMLoadFloat3(&enemyToCheck->GetCurrentPos()) - playerPos));
	float sightRadiusModifier = 1/(_Entities.Size()/(_nrOfStartingEnemies+0.02f));
	if (lengthToPlayer < enemySightRadius*sightRadiusModifier)
	{
		XMVECTOR position = _builder->Transform()->GetPosition(enemyToCheck->GetEntity());
		int testPoint = -1;
		int playerID = -1;
		float xPosition = XMVectorGetX(position), yPosition = XMVectorGetZ(position);
		float playerPositionX = XMVectorGetX(playerPos), playerPositionY = XMVectorGetZ(playerPos);

		if (playerPositionX - floor(playerPositionX) < 0.50f)
		{
			playerID = (int)max(floor(playerPositionX) * 2.0f + floor(playerPositionY)*_sizeOfDungeonSide*2.0f, -1.0f);
		}
		else
		{
			playerID = (int)max(floor(playerPositionX) * 2.0f + floor(playerPositionY)*_sizeOfDungeonSide*2.0f + 1.0f, -1.0f);
		}
		if (playerPositionY - floor(playerPositionY) >= 0.50f)
		{
			playerID += _sizeOfDungeonSide;
		}

		if (xPosition - floor(xPosition) < 0.50f)
		{
			testPoint = (int)max(floor(xPosition) * 2.0f + floor(yPosition)*_sizeOfDungeonSide*2.0f, -1.0f);
		}
		else
		{
			testPoint = (int)max(floor(xPosition) * 2.0f + floor(yPosition)*_sizeOfDungeonSide*2.0f + 1.0f, -1.0f);
		}
		if (yPosition - floor(yPosition) >= 0.50f)
		{
			testPoint += _sizeOfDungeonSide;
}

		if (testPoint == playerID)
		{
			return true;
		}

		bool reachedPlayer = false, foundWall = false;
		int currentID = 0;
		XMVECTOR betweenPlayerAndEnemy = XMVector3Normalize(XMVectorSubtract(playerPos, position));
		float xMovement = XMVectorGetX(betweenPlayerAndEnemy) * 0.05f, yMovement = XMVectorGetZ(betweenPlayerAndEnemy)*0.05f;
		while (!foundWall)
		{
			xPosition += xMovement;
			yPosition += yMovement;
			if (xPosition - floor(xPosition) < 0.50f)
			{
				testPoint = (int)max(floor(xPosition) * 2.0f + floor(yPosition)*_sizeOfDungeonSide*2.0f, -1.0f);
			}
			else
			{
				testPoint = (int)max(floor(xPosition) * 2.0f + floor(yPosition)*_sizeOfDungeonSide*2.0f + 1.0f, -1.0f);
}
			if (yPosition - floor(yPosition) >= 0.50f)
			{
				testPoint += _sizeOfDungeonSide;
			}

			if (testPoint == playerID)
			{
				return true;
			}

			if (!NodeWalkable(xPosition, yPosition))
			{
				foundWall = true;
				return false;
			}

		}
	}

	return false;
}
	
Path* Shodan::NeedPath(Entity entityToGivePath)
{
	XMVECTOR position = _builder->Transform()->GetPosition(entityToGivePath);
	int startPoint = -1;
	float xPosition = XMVectorGetX(position), yPosition = XMVectorGetZ(position);

	if (xPosition - floor(xPosition) < 0.50f)
	{
		startPoint = (int)max(floor(xPosition) * 2.0f + floor(yPosition)*_sizeOfDungeonSide * 2.0f, -1.0f);
	}
	else
	{
		startPoint = (int)max(floor(xPosition) * 2.0f + floor(yPosition)*_sizeOfDungeonSide * 2.0f + 1.0f, -1.0f);
	}
	if (yPosition - floor(yPosition) >= 0.50f)
	{
		startPoint += _sizeOfDungeonSide;
	}

	if (startPoint == -1)
	{
		startPoint = _walkableNodes[rand() % _nrOfWalkableNodesAvailable];
		TraceDebug("Something completely fucked up");
	}


	int goTo = _walkableNodes[rand() % _nrOfWalkableNodesAvailable];


	return  _pathfinding->basicAStar(startPoint, _dungeon[goTo]);;

}

Path* Shodan::NeedPath(Entity entityToGivePath, XMFLOAT3 goal)
{
	XMVECTOR position = _builder->Transform()->GetPosition(entityToGivePath);
	int startPoint = -1, goTo = -1;
	float startPositionX = XMVectorGetX(position), startPositionY = XMVectorGetZ(position);
	float endPositionX = goal.x, endPositionY = goal.z;

	if (startPositionX - floor(startPositionX) < 0.50f)
	{
		startPoint = (int)max(floor(startPositionX) * 2.0f + floor(startPositionY)*_sizeOfDungeonSide * 2.0f, -1.0f);
	}
	else
	{
		startPoint = (int)max(floor(startPositionX) * 2.0f + floor(startPositionY)*_sizeOfDungeonSide * 2.0f + 1.0f, -1.0f);
	}
	if (startPositionY - floor(startPositionY) >= 0.50f)
	{
		startPoint += _sizeOfDungeonSide;
	}

	if (endPositionX - floor(endPositionX) < 0.50f)
	{
		goTo = (int)max(floor(endPositionX) * 2.0f + floor(endPositionY)*_sizeOfDungeonSide * 2.0f, -1.0f);
	}
	else
	{
		goTo = (int)max(floor(endPositionX) * 2.0f + floor(endPositionY)*_sizeOfDungeonSide * 2.0f + 1.0f, -1.0f);
	}
	if (endPositionY - floor(endPositionY) >= 0.50f)
	{
		goTo += _sizeOfDungeonSide;
	}

	Path* test = _pathfinding->basicAStar(startPoint, _dungeon[goTo]);

	return test;

	}

bool Shodan::PlayerSeen()
{
	return _playerSeen;
}
// I think this function is unnecessary
void Shodan::ChangeLightLevel(float lightLevel)
{
	for (int i = 0; i < _Entities.Size(); i++)
	{
		//_builder->Light()->ChangeLightIntensity(_Entities.GetCurrentElement()->_thisEnemy->GetEntity(), lightLevel);
		//_builder->Light()->ChangeLightRange(_Entities.GetCurrentElement()->_thisEnemy->GetEntity(), lightLevel*4.0f);
		_Entities.MoveCurrent();
	}
}

float Shodan::GetLightPoolPercent()
{
	return _lightPoolPercent;
}

void Shodan::CheckCollisionAgainstProjectiles(const vector<Projectile*>& projectiles)
{
	bool didSomeoneDie = false;
	for (int j = 0; j < _Entities.Size(); j++)
	{
		Entity temp = _Entities.GetCurrentElement()->_thisEnemy->GetEntity();
		EnemyWithStates* thisEnemy = _Entities.GetCurrentElement();
		for (int i = 0; i < projectiles.size(); i++)
		{
			if (_builder->Bounding()->CheckCollision(temp, projectiles[i]->GetEntity()))
			{
				// Deal damage
				thisEnemy->_thisEnemyStateController->OnHit(projectiles[i]->GetDamage());
				// Remove projectile so it does not hurt every frame
				projectiles[i]->SetState(false);
				_playerPointer->ShotConnected();
				if (thisEnemy->_thisEnemy->GetHealth() <= 0.0f)
				{
					didSomeoneDie = true;
					_Entities.RemoveCurrentElement();
					if (_Entities.Size() > 0)
					{
						thisEnemy = _Entities.GetCurrentElement();
						temp = _Entities.GetCurrentElement()->_thisEnemy->GetEntity();
				}
			}
		}
		}
		for (int i = 0; i < _playerFriendlyProjectiles.size(); i++)
		{
			if (_builder->Bounding()->CheckCollision(temp, _playerFriendlyProjectiles[i]->GetEntity()))
			{
				if (temp.ID != _playerFriendlyProjectiles[i]->GetOwner().ID)
				{
					// Deal damage
					thisEnemy->_thisEnemyStateController->OnHit(_playerFriendlyProjectiles[i]->GetDamage());
					// Remove projectile so it does not hurt every frame
					_playerFriendlyProjectiles[i]->SetState(false);
					if (thisEnemy->_thisEnemy->GetHealth() <= 0.0f)
					{
						didSomeoneDie = true;
						_Entities.RemoveCurrentElement();
						if (_Entities.Size() > 0)
						{
							thisEnemy = _Entities.GetCurrentElement();
							temp = _Entities.GetCurrentElement()->_thisEnemy->GetEntity();
					}
				}
			}
		}
		}

		_Entities.MoveCurrent();
	}

	if (didSomeoneDie)
	{
		EnemyDied();
	}

}

void Shodan::_CheckIfPlayerIsHit(float deltaTime)
{
	Entity playerEntity = _playerPointer->GetEntity();
	_timeSincePlayerHitSound += deltaTime;
	for (auto &currentProjectile : _enemyProjectiles)
	{
		currentProjectile->Update(deltaTime);
		if (_builder->Bounding()->CheckCollision(currentProjectile->GetEntity(), playerEntity))
		{
			_playerPointer->RemoveHealth(currentProjectile->GetDamage(), _builder->Transform()->GetDirection(currentProjectile->GetEntity()));
			if (_timeSincePlayerHitSound >= 0.25f)
			{
				System::GetAudio()->PlaySoundEffect(L"PlayerHit.wav", 1.0f);
				_timeSincePlayerHitSound = 0.0f;
			}
			currentProjectile->SetState(false);
		}
	}
}

const XMVECTOR& Shodan::PlayerCurrentPosition()
{
	return XMLoadFloat3(&_playerCurrentPosition);
}

bool Shodan::NodeWalkable(float x, float y)
{
	int dungeonID;
	if (x - floor(x) < 0.50f)
	{
		dungeonID = (int)max(floor(x) * 2.0f + floor(y)*_sizeOfDungeonSide*2.0f, -1.0f);
	}
	else
	{
		dungeonID = (int)max(floor(x) * 2.0f + floor(y)*_sizeOfDungeonSide*2.0f + 1.0f, -1.0f);
	}
	if (y - floor(y) >= 0.50f)
	{
		dungeonID += _sizeOfDungeonSide;
	}
	if (dungeonID >= _sizeOfDungeonSide*_sizeOfDungeonSide)
	{
		return false;
	}
	if (dungeonID >= 0 && dungeonID < _sizeOfDungeonSide*_sizeOfDungeonSide * 4)
	{
		return false;
	}
	if (_dungeon[dungeonID]->type != 0)
	{
		return false;
	}

	return true;
}


void Shodan::AddEnemyStartOfLevel(int nrOfEnemiesToSpawn)
{
	float x = _playerCurrentPosition.x, y = _playerCurrentPosition.z;

	for (int i = 0; i < nrOfEnemiesToSpawn; i++)
	{
		int startPoint;
		for (int j = 0; j < 1000; j++)
		{
			startPoint = _walkableNodes[rand() % _nrOfWalkableNodesAvailable];
		}

		float length = sqrt(pow(x - _dungeon[startPoint]->position.x, 2) + pow(y - _dungeon[startPoint]->position.y, 2));
		if (length < enemySightRadius + 5.0f)
		{
			i--;
		}
		else
		{
			_Entities.AddElementToList(_enemyBuilder->AddNewEnemy(XMFLOAT3(_dungeon[startPoint]->position.x + _dungeon[startPoint]->position.offsetX, 0.5f, _dungeon[startPoint]->position.y + _dungeon[startPoint]->position.offsetY)), 0);
		}
	}
	_nrOfStartingEnemies = _Entities.Size();
}
void Shodan::AddEnemyStartOfLevel(EnemyTypes *enemiesTypesToSpawn, int nrOfEnemies, int nrOfEnemiesToSpawn)
{
	float x = _playerCurrentPosition.x, y = _playerCurrentPosition.z;
	for (int i = 0; i < nrOfEnemiesToSpawn; i++)
	{
		int startPoint;
		for (int j = 0; j < 1000; j++)
		{
			startPoint = _walkableNodes[rand() % _nrOfWalkableNodesAvailable];
		}

		float length = sqrt(pow(x - _dungeon[startPoint]->position.x, 2) + pow(y - _dungeon[startPoint]->position.y, 2));
		if (length < 1.0f)//enemySightRadius + 5.0f)
		{
			i--;
		}
		else
		{
			int enemyType = rand() % nrOfEnemies;
			_Entities.AddElementToList(_enemyBuilder->AddNewEnemy(XMFLOAT3(_dungeon[startPoint]->position.x + _dungeon[startPoint]->position.offsetX, 0.5f, _dungeon[startPoint]->position.y + _dungeon[startPoint]->position.offsetY), enemiesTypesToSpawn[enemyType]), 0);
		}
	}
	_nrOfStartingEnemies = _Entities.Size();


}

void Shodan::SetDifficultyBonus(float amount)
{
	for (int i = 0; i < _Entities.Size(); i++)
	{
		_Entities.GetCurrentElement()->_thisEnemy->SetDamageMultiplier(amount);
		_Entities.MoveCurrent();
	}
}

void Shodan::AddEnemy()
{
	int startPoint = _walkableNodes[rand() % _nrOfWalkableNodesAvailable];
	_Entities.AddElementToList(_enemyBuilder->AddNewEnemy(XMFLOAT3(_dungeon[startPoint]->position.x + _dungeon[startPoint]->position.offsetX, 0.5f, _dungeon[startPoint]->position.y + _dungeon[startPoint]->position.offsetY)), 0);
}

void Shodan::AddEnemyAroundPoint(XMFLOAT3 pointToRandomAround, int range)
{
	//NOT TESTED! USE AT YOUR OWN RISK!
	int positionID;
	float x = pointToRandomAround.x, y = pointToRandomAround.z;
	if (x - floor(x) < 0.50f)
	{
		positionID = (int)max(floor(x) * 2.0f + floor(y)*_sizeOfDungeonSide * 2.0f, -1.0f);
	}
	else
	{
		positionID = (int)max(floor(x) * 2.0f + floor(y)*_sizeOfDungeonSide * 2.0f + 1.0f, -1.0f);
	}
	if (y - floor(y) >= 0.50f)
	{
		positionID += _sizeOfDungeonSide;
	}

	int dungeonID = 0;
	int nrOfNodes = 0;
	int *nodesAroundPosition;
	nodesAroundPosition = new int[range*2 + range*4];

	//Alla positioner ovanför
	for (int i = 0; i < range; i++)
	{
		if ((positionID - i*_sizeOfDungeonSide) >= 0)
		{
			//Left side of startPosition.
			dungeonID = positionID - i*_sizeOfDungeonSide;
			for (int j = 1; j < range; j++)
			{
				if ((dungeonID%_sizeOfDungeonSide) != 0)
				{
					if (_dungeon[dungeonID]->type != 0)
					{
						nodesAroundPosition[nrOfNodes] = dungeonID;
						nrOfNodes++;
					}
				}
				else
				{
					j = range;
				}
				dungeonID--;
			}
			//Right side of startPosition
			dungeonID = positionID - i*_sizeOfDungeonSide;
			for (int j = 1; j < range; j++)
			{
				if (((dungeonID+1)%_sizeOfDungeonSide) != 0)
				{
					if (_dungeon[dungeonID]->type != 0)
					{
						nodesAroundPosition[nrOfNodes] = dungeonID;
						nrOfNodes++;
					}
				}
				else
				{
					j = range;
				}
				dungeonID++;
			}
		}
		else
		{
			i = range;
		}
	}

	int maxSize = _sizeOfDungeonSide*_sizeOfDungeonSide;
	for (int i = 0; i < range; i++)
	{
		if ((positionID + i*_sizeOfDungeonSide) < maxSize)
		{
			//Left side of startPosition.
			dungeonID = positionID + i*_sizeOfDungeonSide;
			for (int j = 1; j < range; j++)
			{
				if ((dungeonID%_sizeOfDungeonSide) != 0)
				{
					if (_dungeon[dungeonID]->type != 0)
					{
						nodesAroundPosition[nrOfNodes] = dungeonID;
						nrOfNodes++;
					}
				}
				else
				{
					j = range;
				}
				dungeonID--;
			}
			//Right side of startPosition
			dungeonID = positionID + i*_sizeOfDungeonSide;
			for (int j = 1; j < range; j++)
			{
				if (((dungeonID + 1) % _sizeOfDungeonSide) != 0)
				{
					if (_dungeon[dungeonID]->type != 0)
					{
						nodesAroundPosition[nrOfNodes] = dungeonID;
						nrOfNodes++;
					}
				}
				else
				{
					j = range;
				}
				dungeonID++;
			}
		}
		else
		{
			i = range;
		}
	}


}

void Shodan::_AddEnemyFromListOfPositions(int *nodesToTakeFrom, int nrOfNodes)
{
	if (nrOfNodes)
	{
		int startPoint = nodesToTakeFrom[rand() % nrOfNodes];
		_Entities.AddElementToList(_enemyBuilder->AddNewEnemy(XMFLOAT3(_dungeon[startPoint]->position.x + _dungeon[startPoint]->position.offsetX, 0.5f, _dungeon[startPoint]->position.y + _dungeon[startPoint]->position.offsetY)), 0);
	}
}

List<EnemyWithStates>* Shodan::GetEnemyList()
{
	return &_Entities;
}

Enemy* Shodan::GetClosestEnemy(Entity myEntity)
{
	Enemy* closestEnemy = nullptr;

	float lengthToClosestEnemy = (float)_sizeOfDungeonSide*_sizeOfDungeonSide;
	float lengthToCheck = INFINITY;

	for (int i = 0; i < _Entities.Size(); i++)
	{
		XMFLOAT3 myPosition;
		XMVECTOR myPositionVector = _builder->Transform()->GetPosition(myEntity);
		XMStoreFloat3(&myPosition, myPositionVector);
		XMFLOAT3 thisPosition = _Entities.GetCurrentElement()->_thisEnemy->GetCurrentPos();
		lengthToCheck = sqrt(pow(thisPosition.x - myPosition.x, 2) + pow(thisPosition.z - myPosition.z, 2));
		if (lengthToCheck < lengthToClosestEnemy)
		{
			if (myEntity.ID != _Entities.GetCurrentElement()->_thisEnemy->GetEntity().ID)
			{
				int testPoint = -1;
				int enemyPosition = -1;
				float xPosition = thisPosition.x, yPosition = thisPosition.z;
				float myPositionX = myPosition.x, myPositionY = myPosition.z;

				if (myPositionX - floor(myPositionX) < 0.50f)
				{
					enemyPosition = (int)max(floor(myPositionX) * 2.0f + floor(myPositionY)*_sizeOfDungeonSide*2.0f, -1.0f);
				}
				else
				{
					enemyPosition = (int)max(floor(myPositionX) * 2.0f + floor(myPositionY)*_sizeOfDungeonSide*2.0f + 1.0f, -1.0f);
				}
				if (myPositionY - floor(myPositionY) >= 0.50f)
				{
					enemyPosition += _sizeOfDungeonSide;
				}

				if (xPosition - floor(xPosition) < 0.50f)
				{
					testPoint = (int)max(floor(xPosition) * 2.0f + floor(yPosition)*_sizeOfDungeonSide, -1.0f);
				}
				else
				{
					testPoint = (int)max(floor(xPosition) * 2.0f + floor(yPosition)*_sizeOfDungeonSide*2.0f + 1.0f, -1);
				}
				if (yPosition - floor(yPosition) >= 0.50f)
				{
					testPoint += _sizeOfDungeonSide;
				}

				bool reachedTarget = false, foundWall = false;
				if (testPoint == enemyPosition)
				{
					closestEnemy = _Entities.GetCurrentElement()->_thisEnemy;
					lengthToClosestEnemy = lengthToCheck;
					reachedTarget = true;
				}

				int checkTime = 0;
				XMVECTOR betweenEnemyAndTarget = XMVector3Normalize(XMLoadFloat3(&thisPosition)-myPositionVector);
				float xMovement = XMVectorGetX(betweenEnemyAndTarget)*0.25f, yMovement = XMVectorGetZ(betweenEnemyAndTarget)*0.25f;
				while (!foundWall && !reachedTarget)
				{
					
					checkTime++;
					if (checkTime > 25)
					{
						betweenEnemyAndTarget = XMVector3Normalize(XMLoadFloat3(&thisPosition) - XMLoadFloat3(&XMFLOAT3(myPositionX, 0.5f, myPositionY)));
						xMovement = XMVectorGetX(betweenEnemyAndTarget)*0.25f;
						yMovement = XMVectorGetZ(betweenEnemyAndTarget)*0.25f;
						checkTime = 0;
					}
					myPositionX += xMovement;
					myPositionY += yMovement;
					if (!NodeWalkable(myPositionX, myPositionY))
				{
						foundWall = true;
					}

					if (myPositionX - floor(myPositionX) < 0.50f)
					{
						enemyPosition = (int)max(floor(myPositionX) * 2.0f + floor(myPositionY)*_sizeOfDungeonSide*2.0f, -1.0f);
					}
					else
					{
						enemyPosition = (int)max(floor(myPositionX) * 2.0f + floor(myPositionY)*_sizeOfDungeonSide*2.0f + 1.0f, -1.0f);
					}
					if (myPositionY - floor(myPositionY) >= 0.50f)
					{
						enemyPosition += _sizeOfDungeonSide;
					}

					if (testPoint == enemyPosition)
					{
						closestEnemy = _Entities.GetCurrentElement()->_thisEnemy;
						lengthToClosestEnemy = lengthToCheck;
						reachedTarget = true;
					}
					if (sqrt(pow(thisPosition.x - myPositionX, 2) + pow(thisPosition.z - myPositionY, 2)) < 1.0f)
					{
						closestEnemy = _Entities.GetCurrentElement()->_thisEnemy;
						lengthToClosestEnemy = lengthToCheck;
						reachedTarget = true;
					}

				}
				
			}
		}
		_Entities.MoveCurrent();
	}

	return closestEnemy;

}

void Shodan::EnemyDied()
{
	_lightPoolPercent = (float)((float)_Entities.Size() / (float)_nrOfStartingEnemies);
	_playerPointer->AddLight(_lightPoolPercent);
	_playerPointer->EnemyDefeated();
	float newSize = STARTBLOBRANGELIGHT *0.3f * (_lightPoolPercent)+0.3f;
	float newRange = STARTRANGELIGHT*3.0f * (_lightPoolPercent)+0.3f;
	for (int i = 0; i < _Entities.Size(); i++)
	{
		Entity temp = _Entities.GetCurrentElement()->_thisEnemy->GetEntity();
		_Entities.GetCurrentElement()->_thisEnemyStateController->OnEnemyDeath();
		_builder->Light()->ChangeLightRange(temp, newRange);
		//_builder->Bounding()->CreateBoundingSphere(temp, newSize);
		//_builder->Transform()->SetScale(temp, XMFLOAT3(newSize * scale, newSize * scale, newSize * scale));
		_builder->Light()->ChangeLightBlobRange(temp, newSize);
		_Entities.GetCurrentElement()->_thisEnemyStateController->OnEnemyDeath();
		_Entities.MoveCurrent();
	}
}

void Shodan::EnemyStuck(Entity enemy)
{
	int startPoint = _walkableNodes[rand() % _nrOfWalkableNodesAvailable];

	_builder->Transform()->SetPosition(enemy, XMVectorSet(_dungeon[startPoint]->position.x, 0.5f, _dungeon[startPoint]->position.y, 1.0f));
}