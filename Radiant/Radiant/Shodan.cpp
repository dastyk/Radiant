#include "Shodan.h"
#include "System.h"

#define LengthForUpdate 0.707
using namespace DirectX;
#define STARTINTENSITYLIGHT 3
#define STARTBLOBRANGELIGHT 1.0f
#define STARTRANGELIGHT2 0.5f

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
		Entity newEntity;
		newEntity = _builder->EntityC().Create();

		_builder->Light()->BindPointLight(newEntity, XMFLOAT3(0.0f, 0.0f, 0.0f), STARTRANGELIGHT2, XMFLOAT3((rand() % 200)/100 + 0.5f, (rand() % 200) / 100 + 0.5f, (rand() % 200) / 100 +0.5f), STARTINTENSITYLIGHT);
		_builder->Light()->SetAsVolumetric(newEntity, true);
		_builder->Light()->ChangeLightBlobRange(newEntity, STARTBLOBRANGELIGHT);
		_builder->Transform()->CreateTransform(newEntity);
		_builder->Bounding()->CreateBoundingSphere(newEntity, STARTRANGELIGHT2);
		int startPoint = _walkableNodes[rand() % _nrOfWalkableNodesAvailable];
		_builder->Transform()->SetPosition(newEntity, XMVectorSet(_dungeon[startPoint]->position.x + _dungeon[startPoint]->position.offsetX, 0.5f, _dungeon[startPoint]->position.y + _dungeon[startPoint]->position.offsetY, 1.0f));
		EnemyWithStates* newEnemyWithStates = new EnemyWithStates();
		newEnemyWithStates->_thisEnemy = new Enemy(newEntity, _builder);
		newEnemyWithStates->_thisEnemyStateController = new AIStateController();
		newEnemyWithStates->_thisEnemyStateController->AddState(new AITeleportMoveState(AI_STATE_NONE, this, newEnemyWithStates->_thisEnemy, _builder));
		//newEnemyWithStates->_thisEnemyStateController->AddState(new AIAttackState(AI_STATE_NONE, this, newEnemyWithStates->_thisEnemy, _builder));
		//newEnemyWithStates->_thisEnemyStateController->AddState(new AITransitionState(AI_STATE_NONE, this, newEnemyWithStates->_thisEnemy, _builder));

		_Entities.AddElementToList(newEnemyWithStates, 0);
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
	delete _pathfinding;
	delete _walkableNodes;
	while (_Entities.Size())
	{
		_Entities.RemoveCurrentElement();
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
			_Entities.GetCurrentElement()->_thisEnemyStateController->UpdateMachine(deltaTime);
			}
		_Entities.MoveCurrent();
	}
}

bool Shodan::CheckIfPlayerIsSeenForEnemy(Enemy* enemyToCheck)
{
	float lengthToPlayer = XMVectorGetX(XMVector3Length(XMLoadFloat3(&enemyToCheck->GetCurrentPos()) - _playerCurrentPosition));
	if (lengthToPlayer < enemySightRadius)
	{
		//Very, very, very, very, VERY ugly solution.
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
		startPoint = max(floor(xPosition)*2 + floor(yPosition)*_sizeOfDungeonSide*2, -1);
	}
	else
	{
		startPoint = max(floor(xPosition)*2 + floor(yPosition)*_sizeOfDungeonSide*2 + 1, -1);
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
		_builder->Light()->ChangeLightIntensity(_Entities.GetCurrentElement()->_thisEnemy->GetEntity(), lightLevel);
		_builder->Light()->ChangeLightRange(_Entities.GetCurrentElement()->_thisEnemy->GetEntity(), lightLevel*4.0f);
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
			if (_builder->Bounding()->CheckCollision(projectiles[i]->GetEntity(), temp) > 0)
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

		for (int i = 0; i < _Entities.Size(); i++)
		{

			_builder->Light()->ChangeLightRange(_Entities.GetCurrentElement()->_thisEnemy->GetEntity(), STARTRANGELIGHT2 * (_lightPoolPercent));
			_builder->Bounding()->CreateBoundingSphere(_Entities.GetCurrentElement()->_thisEnemy->GetEntity(), STARTRANGELIGHT2 * (_lightPoolPercent));
			_builder->Light()->ChangeLightBlobRange(_Entities.GetCurrentElement()->_thisEnemy->GetEntity(), STARTBLOBRANGELIGHT * (_lightPoolPercent));
			_Entities.MoveCurrent();
		}
	}
}

void Shodan::CheckIfPlayerIsHit(vector<Projectile*> projectiles)
{
	Entity playerEntity = _playerPointer->GetEntity();
	for (auto &currentProjectile : projectiles)
	{
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