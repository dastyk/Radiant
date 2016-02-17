#include "Shodan.h"

#define LengthForUpdate 0.707

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

	for (int i = 0; i < 25; i++)
	{
		Entity newEntity;
		newEntity = _builder->EntityC().Create();

		_builder->Light()->BindPointLight(newEntity, XMFLOAT3(0.0f, 0.0f, 0.0f), 4, XMFLOAT3((rand() % 200)/100, (rand() % 200) / 100, (rand() % 200) / 100), 10);
		_builder->Light()->SetAsVolumetric(newEntity, true);
		_builder->Transform()->CreateTransform(newEntity);
		int startPoint = _walkableNodes[rand() % _nrOfWalkableNodesAvailable];
		_builder->Transform()->SetPosition(newEntity, XMVectorSet(_dungeon[startPoint]->position.x + 0.5f, 0.5f, _dungeon[startPoint]->position.y + 0.5f, 0.0f));
		_Entities.AddElementToList(new Enemy(newEntity, _builder), 0);
		int goToPoint = rand() % _nrOfWalkableNodesAvailable;
		_Entities.GetCurrentElement()->GivePath(_pathfinding->basicAStar(startPoint, _dungeon[_walkableNodes[goToPoint]]));
		_Entities.GetCurrentElement()->SetSpeedFactor((rand() % 100 + 1) * 0.97f);
	}

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
		Enemy* temp = _Entities.GetCurrentElement();
	//	temp->Update(deltaTime);
		float lengthToPlayer = XMVectorGetX(XMVector3Length(XMLoadFloat3(&temp->GetCurrentPos()) - playerPosition));
		if( lengthToPlayer < _sizeOfDungeonSide*LengthForUpdate)
		{
			if(lengthToPlayer < 5.5f)
			{
				temp->Attack(deltaTime, playerPosition);
			}
			else if (temp->Walking())
			{
				if (!temp->UpdateMovement(deltaTime))
				{
					XMFLOAT3 tempPos = temp->GetCurrentPos();
					int startPoint = floor(tempPos.x) + floor(tempPos.z)*_sizeOfDungeonSide;
					_Entities.GetCurrentElement()->GivePath(_pathfinding->basicAStar(startPoint, _dungeon[_walkableNodes[rand() % _nrOfWalkableNodesAvailable]]));
				}
			}
			else
			{
				XMFLOAT3 tempPos = temp->GetCurrentPos();
				int startPoint = floor(tempPos.x) + floor(tempPos.z)*_sizeOfDungeonSide;
				_Entities.GetCurrentElement()->GivePath(_pathfinding->basicAStar(startPoint, _dungeon[_walkableNodes[rand() % _nrOfWalkableNodesAvailable]]));
			}
		}
		_Entities.MoveCurrent();
	}
}

void Shodan::ChangeLightLevel(float lightLevel)
{
	for (int i = 0; i < _Entities.Size(); i++)
	{
		_builder->Light()->ChangeLightIntensity(_Entities.GetCurrentElement()->GetEntity(), lightLevel);
		_builder->Light()->ChangeLightRange(_Entities.GetCurrentElement()->GetEntity(), lightLevel*4.0f);
		_Entities.MoveCurrent();
	}
}

void Shodan::CheckCollisionAgainstProjectiles(vector<Projectile*> projectiles)
{
	for (int i = 0; i < projectiles.size(); i++)
	{
		for (int j = 0; j < _Entities.Size(); j++)
		{
			if (_builder->Bounding()->CheckCollision(projectiles[i]->GetEntity(), _Entities.GetCurrentElement()->GetEntity()) > 0)
			{
				// Deal damage
				if (_Entities.GetCurrentElement()->ReduceHealth(projectiles[i]->GetDamage()) <= 0)
				{
					_Entities.RemoveCurrentElement();
				}

				// Remove projectile so it does not hurt every frame
				projectiles[i]->SetState(false);
			}
			_Entities.MoveCurrent();
		}
	}
}