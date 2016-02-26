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
	_walkableNodes = new int[sizeOfSide*sizeOfSide*4];
	_nrOfWalkableNodesAvailable = 0;
	_playerPointer = thePlayer;
	_enemyBuilder = new EnemyBuilder(_builder, this);

	MapGridPairs giveMe;
	giveMe.x = 0.0f;	
	giveMe.y = 0.0f;
	//Divide each cell of the dungeon into a 2x2 grid.

	//============
	//== 1 == 2 ==
	//== 3 == 4 ==
	//============

	int j = 0;
	for (int i = 0; i < sizeOfSide*sizeOfSide; i++)
	{
		MapNode* node1 = new MapNode();
		node1->ID = j;
		node1->type = map->getTile(giveMe.x, giveMe.y);
		node1->position = giveMe;
		node1->parentMapNode = i;
		_dungeon[j] = node1;

		MapNode* node2 = new MapNode(*node1);
		node2->position = MapGridPairs(giveMe.x + 0.50f, giveMe.y);
		node2->ID = j + 1;
		node2->parentMapNode = j + 1;
		_dungeon[j + 1] = node2;

		MapNode* node3 = new MapNode(*node1);
		node3->position = MapGridPairs(giveMe.x, giveMe.y + 0.50f);
		node3->ID = j + sizeOfSide * 2;
		node3->parentMapNode = j + sizeOfSide * 2;
		_dungeon[j + sizeOfSide*2] = node3;

		MapNode* node4 = new MapNode(*node1);
		node4->position = MapGridPairs(giveMe.x + 0.50f, giveMe.y + 0.50f);
		node4->ID = j + sizeOfSide * 2 + 1;
		node4->parentMapNode = j + sizeOfSide * 2 + 1;
		_dungeon[j + sizeOfSide * 2 + 1] = node4;

		if (node1->type == 0)
		{
			_walkableNodes[_nrOfWalkableNodesAvailable] = node1->ID;
			_walkableNodes[_nrOfWalkableNodesAvailable + 1] = node2->ID;
			_walkableNodes[_nrOfWalkableNodesAvailable + 2] = node3->ID;
			_walkableNodes[_nrOfWalkableNodesAvailable + 3] = node4->ID;
			_nrOfWalkableNodesAvailable += 4;
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

	_pathfinding = new VeryBasicAI(_dungeon, sizeOfSide*sizeOfSide*4);

	_sizeOfDungeonSide = sizeOfSide * 2;

	for (int i = 0; i < 20; i++)
	{
		AddEnemy();
	}

	_timeUntilWeCheckForPlayer = 2.0f;
	_playerSeen = false;
	_playerSeenAt = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	_nrOfStartingEnemies = _Entities.Size();
	_lightPoolPercent = 1.0f;

}

Shodan::~Shodan()
{
	for (int i = 0; i < _sizeOfDungeonSide*_sizeOfDungeonSide; i++)
	{
		delete _dungeon[i];
	}
	delete[] _dungeon;
	SAFE_DELETE(_pathfinding);
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
}

void Shodan::Update(float deltaTime, XMVECTOR playerPosition)
{
	_playerCurrentPosition = playerPosition;

	for (int i = 0; i < _Entities.Size(); i++)
	{
		Enemy* temp = _Entities.GetCurrentElement()->_thisEnemy;
		float lengthToPlayer = XMVectorGetX(XMVector3Length(XMLoadFloat3(&temp->GetCurrentPos()) - playerPosition));
		if (lengthToPlayer < _sizeOfDungeonSide*LengthForUpdate)
				{
			vector<Projectile*> temp = _Entities.GetCurrentElement()->_thisEnemy->GetWeapon()->GetProjectilesOwnership();
			if (temp.size())
			{
				_enemyProjectiles.insert(_enemyProjectiles.end(), temp.begin(), temp.end());
			}
			_Entities.GetCurrentElement()->_thisEnemyStateController->UpdateMachine(deltaTime);

			}
		_Entities.MoveCurrent();
	}
	for (int i = 0; i < _enemyProjectiles.size(); i++)
	{
		XMVECTOR temp = _builder->Transform()->GetPosition(_enemyProjectiles[i]->GetEntity());
		float yPosition = XMVectorGetY(temp);
		float lightRange = _builder->Light()->GetLightRange(_enemyProjectiles[i]->GetEntity());
		if (!NodeWalkable(XMVectorGetX(temp)+lightRange, XMVectorGetZ(temp)+lightRange) || yPosition < 0.0f || yPosition > 3.5f)
			_enemyProjectiles[i]->SetState(false);

		if (!_enemyProjectiles[i]->GetState())
		{
			delete _enemyProjectiles[i];
			_enemyProjectiles.erase(_enemyProjectiles.begin() + i);
			_enemyProjectiles.shrink_to_fit();
			i--;
		}
}
	_CheckIfPlayerIsHit(deltaTime);


	_lightPoolPercent = (float)((float)_Entities.Size() / (float)_nrOfStartingEnemies); // ---------------------------------------
	float newSize = STARTBLOBRANGELIGHT * (_lightPoolPercent);
	float newRange = STARTRANGELIGHT*3.0 * (_lightPoolPercent);
	for (int i = 0; i < _Entities.Size(); i++)
	{
		Entity temp = _Entities.GetCurrentElement()->_thisEnemy->GetEntity();
		_builder->Light()->ChangeLightRange(temp, newRange);
		_builder->Transform()->SetScale(temp, XMFLOAT3(newSize, newSize, newSize));
		_builder->Light()->ChangeLightBlobRange(temp, newSize);
		_Entities.MoveCurrent();
	}

}

bool Shodan::CheckIfPlayerIsSeenForEnemy(Enemy* enemyToCheck)
{
	float lengthToPlayer = XMVectorGetX(XMVector3Length(XMLoadFloat3(&enemyToCheck->GetCurrentPos()) - _playerCurrentPosition));
	if (lengthToPlayer < enemySightRadius)
	{
		XMVECTOR position = _builder->Transform()->GetPosition(enemyToCheck->GetEntity());
		int testPoint = -1;
		int playerID = -1;
		float xPosition = XMVectorGetX(position), yPosition = XMVectorGetZ(position);
		float playerPositionX = XMVectorGetX(_playerCurrentPosition)-0.50f, playerPositionY = XMVectorGetZ(_playerCurrentPosition)-0.50f;

		if (playerPositionX - floor(playerPositionX) < 0.50f)
		{
			playerID = max(floor(playerPositionX) * 2 + floor(playerPositionY)*_sizeOfDungeonSide * 2, -1);
		}
		else
		{
			playerID = max(floor(playerPositionX) * 2 + floor(playerPositionY)*_sizeOfDungeonSide * 2 + 1, -1);
		}
		if (playerPositionY - floor(playerPositionY) >= 0.50f)
		{
			playerID += _sizeOfDungeonSide;
		}

		if (xPosition - floor(xPosition) < 0.50f)
		{
			testPoint = max(floor(xPosition) * 2 + floor(yPosition)*_sizeOfDungeonSide * 2, -1);
		}
		else
		{
			testPoint = max(floor(xPosition) * 2 + floor(yPosition)*_sizeOfDungeonSide * 2 + 1, -1);
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
		XMVECTOR betweenPlayerAndEnemy = XMVector3Normalize(XMVectorSubtract(_playerCurrentPosition, position));
		float xMovement = XMVectorGetX(betweenPlayerAndEnemy) * 0.5f, yMovement = XMVectorGetZ(betweenPlayerAndEnemy)*0.5f;
		while (!foundWall)
		{
			xPosition += xMovement;
			yPosition += yMovement;
			if (xPosition - floor(xPosition) < 0.50f)
			{
				testPoint = max(floor(xPosition) * 2 + floor(yPosition)*_sizeOfDungeonSide * 2, -1);
			}
			else
			{
				testPoint = max(floor(xPosition) * 2 + floor(yPosition)*_sizeOfDungeonSide * 2 + 1, -1);
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
		startPoint = max(floor(xPosition) * 2 + floor(yPosition)*_sizeOfDungeonSide * 2, -1);
	}
	else
	{
		startPoint = max(floor(xPosition) * 2 + floor(yPosition)*_sizeOfDungeonSide * 2 + 1, -1);
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
	Path* test = _pathfinding->basicAStar(startPoint, _dungeon[goTo]);

	return test;
	
}

Path* Shodan::NeedPath(Entity entityToGivePath, XMFLOAT3 goal)
{
	XMVECTOR position = _builder->Transform()->GetPosition(entityToGivePath);
	int startPoint = -1, goTo = -1;
	float startPositionX = XMVectorGetX(position), startPositionY = XMVectorGetZ(position);
	float endPositionX = goal.x, endPositionY = goal.z;

	if (startPositionX - floor(startPositionX) < 0.50f)
	{
		startPoint = max(floor(startPositionX) * 2 + floor(startPositionY)*_sizeOfDungeonSide * 2, -1);
	}
	else
	{
		startPoint = max(floor(startPositionX) * 2 + floor(startPositionY)*_sizeOfDungeonSide * 2 + 1, -1);
	}
	if (startPositionY - floor(startPositionY) >= 0.50f)
	{
		startPoint += _sizeOfDungeonSide;
	}

	if (endPositionX - floor(endPositionX) < 0.50f)
	{
		goTo = max(floor(endPositionX) * 2 + floor(endPositionY)*_sizeOfDungeonSide * 2, -1);
	}
	else
	{
		goTo = max(floor(endPositionX) * 2 + floor(endPositionY)*_sizeOfDungeonSide * 2 + 1, -1);
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

void Shodan::CheckCollisionAgainstProjectiles(vector<Projectile*> projectiles)
{
	bool didSomeoneDie = false;

		for (int j = 0; j < _Entities.Size(); j++)
		{
		Entity temp = _Entities.GetCurrentElement()->_thisEnemy->GetEntity();
	for (int i = 0; i < projectiles.size(); i++)
			{
			if (_builder->Bounding()->CheckCollision(temp, projectiles[i]->GetEntity()))
				{
				/*if (_Entities.GetCurrentElement()->_thisEnemy->GetTimeSinceLastSound() >= 5.0f)
				{
					int tempNr = rand() % 5 + 1;

					if (tempNr == 1)
						System::GetAudio()->PlaySoundEffect(L"DamageSound1.wav", 1);
					else if (tempNr == 2)
						System::GetAudio()->PlaySoundEffect(L"DamageSound2.wav", 1);
					else if (tempNr == 3)
						System::GetAudio()->PlaySoundEffect(L"DamageSound3.wav", 1);
					else if (tempNr == 4)
						System::GetAudio()->PlaySoundEffect(L"DamageSound4.wav", 1);
					else if (tempNr == 5)
						System::GetAudio()->PlaySoundEffect(L"DamageSound5.wav", 1);


					_Entities.GetCurrentElement()->_thisEnemy->ResetTimeSinceLastSound();
				}*/
				// Deal damage
				if (_Entities.GetCurrentElement()->_thisEnemy->ReduceHealth(projectiles[i]->GetDamage()) <= 0)
				{
					didSomeoneDie = true;
					_Entities.RemoveCurrentElement();
				}

				// Remove projectile so it does not hurt every frame
				projectiles[i]->SetState(false);
			}
		}

			_Entities.MoveCurrent();
		}

	if (didSomeoneDie)
	{
		_lightPoolPercent = (float)((float)_Entities.Size() / (float)_nrOfStartingEnemies);
		float newSize = STARTBLOBRANGELIGHT * (_lightPoolPercent);
		float newRange = STARTRANGELIGHT*3.0 * (_lightPoolPercent);
		for (int i = 0; i < _Entities.Size(); i++)
		{
			Entity temp = _Entities.GetCurrentElement()->_thisEnemy->GetEntity();
			_builder->Light()->ChangeLightRange(temp, newRange);
			_builder->Transform()->SetScale(temp, XMFLOAT3(newSize, newSize, newSize));
			_builder->Light()->ChangeLightBlobRange(temp, newSize);
			_Entities.MoveCurrent();
		}
	}
}

void Shodan::_CheckIfPlayerIsHit(float deltaTime)
{
	Entity playerEntity = _playerPointer->GetEntity();
	for (auto &currentProjectile : _enemyProjectiles)
	{
		currentProjectile->Update(deltaTime);
		if (_builder->Bounding()->CheckCollision(currentProjectile->GetEntity(), playerEntity))
		{
			_playerPointer->RemoveHealth(currentProjectile->GetDamage());
			currentProjectile->SetState(false);
		}
	}
}

XMVECTOR Shodan::PlayerCurrentPosition()
{
	return _playerCurrentPosition;
}

bool Shodan::NodeWalkable(float x, float y)
{
	int dungeonID;
	if (x - floor(x) < 0.50f)
	{
		dungeonID = max(floor(x) * 2 + floor(y)*_sizeOfDungeonSide * 2, -1);
	}
	else
	{
		dungeonID = max(floor(x) * 2 + floor(y)*_sizeOfDungeonSide * 2 + 1, -1);
	}
	if (y - floor(y) >= 0.50f)
	{
		dungeonID += _sizeOfDungeonSide;
	}

	if (_dungeon[dungeonID]->type != 0)
	{
		return false;
	}

	return true;
}

void Shodan::AddEnemy()
{
	int startPoint = _walkableNodes[rand() % _nrOfWalkableNodesAvailable];
	_Entities.AddElementToList(_enemyBuilder->AddNewEnemy(XMFLOAT3(_dungeon[startPoint]->position.x + _dungeon[startPoint]->position.offsetX, 0.5f, _dungeon[startPoint]->position.y + _dungeon[startPoint]->position.offsetY)), 0);
}

List<EnemyWithStates>* Shodan::GetEnemyList()
{
	return &_Entities;
}