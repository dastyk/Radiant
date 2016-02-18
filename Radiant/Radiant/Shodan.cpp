#include "Shodan.h"
#include "System.h"

#define LengthForUpdate 0.707
#define STARTINTENSITYLIGHT 3

#define STARTRANGELIGHT2 1.0f

Shodan::Shodan()
{

}

Shodan::Shodan(EntityBuilder* builder, Dungeon* map, int sizeOfSide) : _builder(builder)
{
	_sizeOfDungeonSide = sizeOfSide;
	_dungeon = new MapNode*[sizeOfSide*sizeOfSide];
	_walkableNodes = new int[sizeOfSide*sizeOfSide];
	_nrOfWalkableNodesAvailable = 0;

	Pair giveMe;
	giveMe.x = 0;
	giveMe.y = 0;
	for (int i = 0; i < sizeOfSide*sizeOfSide; i++)
	{
		MapNode* temp = new MapNode();
		temp->ID = i;
		temp->type = map->getTile(giveMe.x, giveMe.y);
		if (temp->type == 0)
		{
			_walkableNodes[_nrOfWalkableNodesAvailable] = i;
			_nrOfWalkableNodesAvailable++;
		}
		temp->position = giveMe;
		temp->parentMapNode = i;
		_dungeon[i] = temp;
		giveMe.x++;
		if (!(giveMe.x % (sizeOfSide)))
		{
			giveMe.y++;
			giveMe.x = 0;
		}
	}
	_pathfinding = new VeryBasicAI(_dungeon, sizeOfSide*sizeOfSide);

	for (int i = 0; i < 20; i++)
	{
		Entity newEntity;
		newEntity = _builder->EntityC().Create();

		_builder->Light()->BindPointLight(newEntity, XMFLOAT3(0.0f, 0.0f, 0.0f), STARTRANGELIGHT2, XMFLOAT3((rand() % 200)/100 + 0.5f, (rand() % 200) / 100 + 0.5f, (rand() % 200) / 100 +0.5f), STARTINTENSITYLIGHT);
		_builder->Light()->SetAsVolumetric(newEntity, true);
		_builder->Transform()->CreateTransform(newEntity);
		_builder->Bounding()->CreateBoundingSphere(newEntity, STARTRANGELIGHT2);
		int startPoint = _walkableNodes[rand() % _nrOfWalkableNodesAvailable];
		_builder->Transform()->SetPosition(newEntity, XMVectorSet(_dungeon[startPoint]->position.x, 0.5f, _dungeon[startPoint]->position.y, 0.0f));
		EnemyWithStates* newEnemyWithStates = new EnemyWithStates();
		newEnemyWithStates->_thisEnemy = new Enemy(newEntity, _builder);
		newEnemyWithStates->_thisEnemyStateController = new AIStateController();
		newEnemyWithStates->_thisEnemyStateController->AddState(new AIPatrolState(AI_STATE_NONE, this, newEnemyWithStates->_thisEnemy, _builder));

		_Entities.AddElementToList(newEnemyWithStates, 0);
	}

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


Path* Shodan::NeedPath(Entity entityToGivePath)
	{

	int goTo = _walkableNodes[rand() % _nrOfWalkableNodesAvailable];
	XMVECTOR position = _builder->Transform()->GetPosition(entityToGivePath);

	int startPoint = floor(XMVectorGetX(position)) + floor(XMVectorGetZ(position))*_sizeOfDungeonSide;
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

	for (int i = 0; i < projectiles.size(); i++)
	{
		for (int j = 0; j < _Entities.Size(); j++)
		{
			if (_builder->Bounding()->CheckCollision(projectiles[i]->GetEntity(), _Entities.GetCurrentElement()->_thisEnemy->GetEntity()) > 0)
			{
				if (_Entities.GetCurrentElement()->_thisEnemy->GetTimeSinceLastSound() >= 5.0f)
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
				}
				// Deal damage
				if (_Entities.GetCurrentElement()->_thisEnemy->ReduceHealth(projectiles[i]->GetDamage()) <= 0)
				{
					didSomeoneDie = true;
					_Entities.RemoveCurrentElement();
				}

				


				// Remove projectile so it does not hurt every frame
				projectiles[i]->SetState(false);
			}
			_Entities.MoveCurrent();
		}
	}

	if (didSomeoneDie)
	{
		_lightPoolPercent = (float)((float)_Entities.Size() / (float)_nrOfStartingEnemies);

		for (int i = 0; i < _Entities.Size(); i++)
		{

			_builder->Light()->ChangeLightRange(_Entities.GetCurrentElement()->_thisEnemy->GetEntity(), STARTRANGELIGHT2 * _lightPoolPercent);
			_builder->Light()->ChangeLightIntensity(_Entities.GetCurrentElement()->_thisEnemy->GetEntity(), STARTINTENSITYLIGHT * _lightPoolPercent);
			_builder->Bounding()->CreateBoundingSphere(_Entities.GetCurrentElement()->_thisEnemy->GetEntity(), STARTRANGELIGHT2 * _lightPoolPercent);
			_Entities.MoveCurrent();
		}
	}
}