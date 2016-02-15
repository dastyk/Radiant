#include "Shodan.h"


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

void Shodan::Update(float deltaTime)
{
	for (int i = 0; i < _Entities.Size(); i++)
	{
		if (_Entities.GetCurrentElement()->Walking())
		{
			_Entities.GetCurrentElement()->Update(deltaTime);
		}
		else
		{
			XMFLOAT3 temp = _Entities.GetCurrentElement()->GetCurrentPos();
			int startPoint = floor(temp.x - 0.5f) + floor(temp.z - 0.5f)*_sizeOfDungeonSide;
			_Entities.GetCurrentElement()->GivePath(_pathfinding->basicAStar(startPoint, _dungeon[_walkableNodes[rand() % _nrOfWalkableNodesAvailable]]));
		}
		_Entities.MoveCurrent();
	}
}