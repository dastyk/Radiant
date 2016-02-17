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
		EnemyWithStates* newEnemyWithStates = new EnemyWithStates();
		newEnemyWithStates->_thisEnemy = new Enemy(newEntity, _builder);
		newEnemyWithStates->_thisEnemyStateController = new AIStateController();
		newEnemyWithStates->_thisEnemyStateController->AddState(new AIPatrolState(AI_STATE_NONE, this, newEnemyWithStates->_thisEnemy, _builder));

		_Entities.AddElementToList(newEnemyWithStates, 0);
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
		Enemy* temp = _Entities.GetCurrentElement()->_thisEnemy;
		float lengthToPlayer = XMVectorGetX(XMVector3Length(XMLoadFloat3(&temp->GetCurrentPos()) - playerPosition));
		if (lengthToPlayer < _sizeOfDungeonSide*LengthForUpdate)
		{
			_Entities.GetCurrentElement()->_thisEnemyStateController->UpdateMachine(deltaTime);
		}
		_Entities.MoveCurrent();
	}
}

void Shodan::ChangeLightLevel(float lightLevel)
{
	
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