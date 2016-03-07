#include "VeryBasicAI.h"


VeryBasicAI::VeryBasicAI(MapNode** _mapGrid, int size)
{
	this->_size = size;
	this->_maxSize = (int)sqrt(size);
	this->_mapGrid = new MapNode*[size];
	for (int i = 0; i < size; i++)
	{
		this->_mapGrid[i] = _mapGrid[i];
		if (this->_mapGrid[i]->type == 0)
		{
			if ((i%_maxSize) != 0)
			{
				if (_mapGrid[i - 1]->type != 0)
				{
					_mapGrid[i]->position.offsetX += wallOffsetOther;
				}
			}
			if ((i + 1) % _maxSize != 0)
			{
				if (_mapGrid[i + 1]->type != 0)
				{
					_mapGrid[i]->position.offsetX -= wallOffsetOther;
				}
			}
			if (!((i + _maxSize) > size))
			{
				if (_mapGrid[i + _maxSize]->type != 0)
				{
					_mapGrid[i]->position.offsetY -= wallOffsetOther;
				}
				if (!((i) % _maxSize != 0))
				{
					if (_mapGrid[i + _maxSize - 1]->type != 0)
					{
						_mapGrid[i]->position.offsetY -= wallOffsetDiagonal;
						_mapGrid[i]->position.offsetX += wallOffsetDiagonal;
					}
				}
				if (!((i + 1) % _maxSize != 0))
				{
					if (!(i + _maxSize + 1 > size))
					{
						if (_mapGrid[i + _maxSize + 1]->type != 0)
						{
							_mapGrid[i]->position.offsetY -= wallOffsetDiagonal;
							_mapGrid[i]->position.offsetX -= wallOffsetDiagonal;
						}
					}
				}
			}
		}
		if (!((i - _maxSize) < 0))
		{
			if (_mapGrid[i - _maxSize]->type != 0)
			{
				_mapGrid[i]->position.offsetY += wallOffsetOther;
			}
			if (!((i + 1) % _maxSize != 0))
			{
				if (_mapGrid[i - _maxSize + 1]->type != 0)
				{
					_mapGrid[i]->position.offsetX -= wallOffsetDiagonal;
					_mapGrid[i]->position.offsetY += wallOffsetDiagonal;
				}
			}
			if (!((i) % _maxSize) != 0)
			{
				if (!(i - _maxSize - 1 < 0))
				{
					if (_mapGrid[i - _maxSize - 1]->type != 0)
					{
						_mapGrid[i]->position.offsetX += wallOffsetDiagonal;
						_mapGrid[i]->position.offsetY += wallOffsetDiagonal;
					}
				}
			}
		}
	}
	_closedList = new bool[size]; //Worst case scenario (can't happen)
	_openList = new bool[size]; //Same as above.
	_openNodeList = new MapNode*[size];
	_sizeOfOpenList = 0;

}

VeryBasicAI::~VeryBasicAI()
{
	delete[] _closedList;
	delete[] _openList;
	delete[] _mapGrid;
	delete[] _openNodeList;
}

void VeryBasicAI::_calculateHCost()	//Size is the size of the entire MapNode array, that is size*size of the room array.
{
	for (int i = 0; i < _size; i++)
	{
		_mapGrid[i]->hValue = 0;

		if (_mapGrid[i]->type != 0)
		{
			_mapGrid[i]->hValue = -1;
		}
		else if (_mapGrid[i]->ID == _targetMapNode->ID)
		{

		}
		else if (_mapGrid[i]->position.x < _targetMapNode->position.x)
		{
			_mapGrid[i]->hValue += (int)(_targetMapNode->position.x - _mapGrid[i]->position.x);
			if (_mapGrid[i]->position.y < _targetMapNode->position.y)
			{
				_mapGrid[i]->hValue += (int)(_targetMapNode->position.y - _mapGrid[i]->position.y)*hValueMultiplier;
			}
			else
			{
				_mapGrid[i]->hValue += (int)(_mapGrid[i]->position.y - _targetMapNode->position.y)*hValueMultiplier;
			}
		}
		else
		{
			_mapGrid[i]->hValue += (int)(_mapGrid[i]->position.x - _targetMapNode->position.x);
			if (_mapGrid[i]->position.y < _targetMapNode->position.y)
			{
				_mapGrid[i]->hValue += (int)(_targetMapNode->position.y - _mapGrid[i]->position.y)*hValueMultiplier;
			}
			else
			{
				_mapGrid[i]->hValue += (int)(_mapGrid[i]->position.y - _targetMapNode->position.y)*hValueMultiplier;
			}
		}
	}
}


bool VeryBasicAI::_evaluateAroundMapNode()
{
	//When we evaluate a node, we check the nodes around it, to find the "cheapest" way.
	//This is done by moving the "nodeToBeEvaluated", check if this node is closed (already checked, a wall or void) and, if not, check if it has been evaluated previously.
	//If it has not been evaluated previously, or if it's evaluated G value is lower than the G value we get by going through th� _currentMapNode, we set the gValue to either
	//_currentMapNode->gValue + 10, if the way to the point is straight, or _currentMapNode->gValue + 14, if diagonal.

	//We start with the MapNode above
	if (_currentMapNode->ID - _maxSize > 0)
	{
		_nodeToBeEvaluated = _mapGrid[_currentMapNode->ID - _maxSize];
		if (_nodeToBeEvaluated == _targetMapNode)
		{
			_mapGrid[_targetMapNode->ID]->parentMapNode = _currentMapNode->ID;
			return true;
		}
		if (!_closedList[_nodeToBeEvaluated->ID])
		{
			if (!_openList[_nodeToBeEvaluated->ID])
			{
				_nodeToBeEvaluated->gValue = _currentMapNode->gValue + straightMoveCost;
				_nodeToBeEvaluated->calculateF();
				_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
				_openList[_nodeToBeEvaluated->ID] = 1;
				_openNodeList[_sizeOfOpenList] = _nodeToBeEvaluated;
				_sizeOfOpenList++;

			}
			else if (_nodeToBeEvaluated->gValue > _currentMapNode->gValue + straightMoveCost)
			{
				_nodeToBeEvaluated->gValue = _currentMapNode->gValue + straightMoveCost;
				_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
				_nodeToBeEvaluated->calculateF();
			}
		}
		//MapNode above to the left
		if ((_currentMapNode->ID - _maxSize) % _maxSize)
		{
			_nodeToBeEvaluated = _mapGrid[_currentMapNode->ID - _maxSize - 1];
			if (_nodeToBeEvaluated == _targetMapNode)
			{
				_mapGrid[_targetMapNode->ID]->parentMapNode = _currentMapNode->ID;
				return true;
			}
			if (!_closedList[_nodeToBeEvaluated->ID])
			{
				if (!_openList[_nodeToBeEvaluated->ID])
				{
					_nodeToBeEvaluated->gValue = _currentMapNode->gValue + diagonalMoveCost;
					_nodeToBeEvaluated->calculateF();
					_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
					_openList[_nodeToBeEvaluated->ID] = 1;
					_openNodeList[_sizeOfOpenList] = _nodeToBeEvaluated;
					_sizeOfOpenList++;

				}
				else if (_nodeToBeEvaluated->gValue > _currentMapNode->gValue + diagonalMoveCost)
				{
					_nodeToBeEvaluated->gValue = _currentMapNode->gValue + diagonalMoveCost;
					_nodeToBeEvaluated->calculateF();
					_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
				}
			}
		}
		//MapNode above to the right
		if ((_currentMapNode->ID - _maxSize + 1) % _maxSize)
		{
			_nodeToBeEvaluated = _mapGrid[_currentMapNode->ID - _maxSize + 1];
			if (_nodeToBeEvaluated == _targetMapNode)
			{
				_mapGrid[_targetMapNode->ID]->parentMapNode = _currentMapNode->ID;
				return true;
			}
			if (!_closedList[_nodeToBeEvaluated->ID])
			{
				if (!_openList[_nodeToBeEvaluated->ID])
				{
					_nodeToBeEvaluated->gValue = _currentMapNode->gValue + diagonalMoveCost;
					_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
					_nodeToBeEvaluated->calculateF();
					_openList[_nodeToBeEvaluated->ID] = 1;
					_openNodeList[_sizeOfOpenList] = _nodeToBeEvaluated;
					_sizeOfOpenList++;
				}
				else if (_nodeToBeEvaluated->gValue > _currentMapNode->gValue + diagonalMoveCost)
				{
					_nodeToBeEvaluated->gValue = _currentMapNode->gValue + diagonalMoveCost;
					_nodeToBeEvaluated->calculateF();
					_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
				}
			}
		}
	}

	//MapNode below
	if (_currentMapNode->ID < _size - _maxSize)
	{
		_nodeToBeEvaluated = _mapGrid[_currentMapNode->ID + _maxSize];
		if (_nodeToBeEvaluated == _targetMapNode)
		{
			_mapGrid[_targetMapNode->ID]->parentMapNode = _currentMapNode->ID;
			return true;
		}
		if (!_closedList[_nodeToBeEvaluated->ID])
		{
			if (!_openList[_nodeToBeEvaluated->ID])
			{
				_nodeToBeEvaluated->gValue = _currentMapNode->gValue + straightMoveCost;
				_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
				_nodeToBeEvaluated->calculateF();
				_openList[_nodeToBeEvaluated->ID] = 1;
				_openNodeList[_sizeOfOpenList] = _nodeToBeEvaluated;
				_sizeOfOpenList++;
			}
			else if (_nodeToBeEvaluated->gValue > _currentMapNode->gValue + straightMoveCost)
			{
				_mapGrid[_currentMapNode->ID + _maxSize]->gValue = _currentMapNode->gValue + straightMoveCost;
				_nodeToBeEvaluated->calculateF();
				_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
			}
		}
		//MapNode below to the left
		if ((_currentMapNode->ID + _maxSize) % _maxSize)
		{
			_nodeToBeEvaluated = _mapGrid[_currentMapNode->ID + _maxSize - 1];
			if (_nodeToBeEvaluated == _targetMapNode)
			{
				_mapGrid[_targetMapNode->ID]->parentMapNode = _currentMapNode->ID;
				return true;
			}
			if (!_closedList[_nodeToBeEvaluated->ID])
			{
				if (!_openList[_nodeToBeEvaluated->ID])
				{
					_nodeToBeEvaluated->gValue = _currentMapNode->gValue + diagonalMoveCost;
					_nodeToBeEvaluated->calculateF();
					_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
					_openList[_nodeToBeEvaluated->ID] = 1;
					_openNodeList[_sizeOfOpenList] = _nodeToBeEvaluated;
					_sizeOfOpenList++;
				}
				else if (_nodeToBeEvaluated->gValue > _currentMapNode->gValue + diagonalMoveCost)
				{
					_nodeToBeEvaluated->gValue = _currentMapNode->gValue + diagonalMoveCost;
					_nodeToBeEvaluated->calculateF();
					_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
				}
			}
		}

		//MapNode below to the right
		if ((_currentMapNode->ID + _maxSize + 1) % _maxSize)
		{
			_nodeToBeEvaluated = _mapGrid[_currentMapNode->ID + _maxSize + 1];
			if (_nodeToBeEvaluated == _targetMapNode)
			{
				_mapGrid[_targetMapNode->ID]->parentMapNode = _currentMapNode->ID;
				return true;
			}

			if (!_closedList[_nodeToBeEvaluated->ID])
			{
				if (!_openList[_nodeToBeEvaluated->ID])
				{
					_nodeToBeEvaluated->gValue = _currentMapNode->gValue + diagonalMoveCost;
					_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
					_nodeToBeEvaluated->calculateF();
					_openList[_nodeToBeEvaluated->ID] = 1;
					_openNodeList[_sizeOfOpenList] = _nodeToBeEvaluated;
					_sizeOfOpenList++;

				}
				else if (_nodeToBeEvaluated->gValue > _currentMapNode->gValue + diagonalMoveCost)
				{
					_nodeToBeEvaluated->gValue = _currentMapNode->gValue + diagonalMoveCost;
					_nodeToBeEvaluated->calculateF();
					_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
				}
			}
		}
	}

	//MapNode to the right
	if ((_currentMapNode->ID + 1) % _maxSize)
	{
		_nodeToBeEvaluated = _mapGrid[_currentMapNode->ID + 1];
		if (_nodeToBeEvaluated == _targetMapNode)
		{
			_mapGrid[_targetMapNode->ID]->parentMapNode = _currentMapNode->ID;
			return true;
		}

		if (!_closedList[_nodeToBeEvaluated->ID])
		{
			if (!_openList[_nodeToBeEvaluated->ID])
			{
				_nodeToBeEvaluated->gValue = _currentMapNode->gValue + straightMoveCost;
				_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
				_nodeToBeEvaluated->calculateF();
				_openList[_nodeToBeEvaluated->ID] = 1;
				_openNodeList[_sizeOfOpenList] = _nodeToBeEvaluated;
				_sizeOfOpenList++;
			}
			else if (_nodeToBeEvaluated->gValue > _currentMapNode->gValue + straightMoveCost)
			{
				_nodeToBeEvaluated->gValue = _currentMapNode->gValue + straightMoveCost;
				_nodeToBeEvaluated->calculateF();
				_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
			}
		}
	}

	//MapNode to the left
	if ((_currentMapNode->ID) % _maxSize)
	{
		_nodeToBeEvaluated = _mapGrid[_currentMapNode->ID - 1];
		if (_nodeToBeEvaluated == _targetMapNode)
		{
			_mapGrid[_targetMapNode->ID]->parentMapNode = _currentMapNode->ID;
			_nodeToBeEvaluated->calculateF();
			return true;
		}

		if (!_closedList[_nodeToBeEvaluated->ID])
		{
			if (!_openList[_nodeToBeEvaluated->ID])
			{
				_nodeToBeEvaluated->gValue = _currentMapNode->gValue + straightMoveCost;
				_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
				_nodeToBeEvaluated->calculateF();
				_openList[_nodeToBeEvaluated->ID] = 1;
				_openNodeList[_sizeOfOpenList] = _nodeToBeEvaluated;
				_sizeOfOpenList++;
			}
			else if (_nodeToBeEvaluated->gValue > _currentMapNode->gValue + straightMoveCost)
			{
				_nodeToBeEvaluated->gValue = _currentMapNode->gValue + straightMoveCost;
				_nodeToBeEvaluated->calculateF();
				_nodeToBeEvaluated->parentMapNode = _currentMapNode->ID;
			}
		}
	}

	return false;
}

//Function to add all walls and void to the closed, so they're never evaluated. Returns the number of closed objects.
//Not that the function takes for granted that the "_closedList" array is empty.
void VeryBasicAI::_initializeClosedList()
{
	for (int i = 0; i < _size; i++)
	{
		if (_mapGrid[i]->type)
		{
			_closedList[i] = 1;
		}
		else
		{
			_closedList[i] = 0;
		}

	}

}

void VeryBasicAI::_sortOpenList()
{
	MapNode* temp;
	int position;
	temp = _openNodeList[_sizeOfOpenList - 1];
	position = _sizeOfOpenList - 1;
	for (int i = _sizeOfOpenList - 2; i > 0; i--)
	{
		if (_openNodeList[i]->fValue < temp->fValue)
		{
			temp = _openNodeList[i];
			position = i;
		}
	}
	_openNodeList[position] = _openNodeList[_sizeOfOpenList - 1];
	_openNodeList[_sizeOfOpenList - 1] = temp;

}

//Returnera en "lista" �ver punkter att r�ra sig till, samt hur m�nga punkter som skall r�ras sig igenom.
Path* VeryBasicAI::basicAStar(int startPosition, MapNode *targetMapNode)
{
	
	clear();
	_initializeClosedList();

	if (_closedList[startPosition])
		return nullptr;

	if (_closedList[targetMapNode->ID])
		return nullptr;

	this->_targetMapNode = targetMapNode;
	_calculateHCost();

	_currentMapNode = _mapGrid[startPosition];
	_currentMapNode->gValue = 0;
	_closedList[_currentMapNode->ID] = 1;

	while (!_evaluateAroundMapNode())
	{
		if (_sizeOfOpenList <= 0)
		{
			return nullptr;
		}
		_sortOpenList();
		_sizeOfOpenList--;
		_currentMapNode = _openNodeList[_sizeOfOpenList];
		_closedList[_currentMapNode->ID] = 1;
		_openList[_currentMapNode->ID] = 0;
	}
	Path* returnPath;
	returnPath = new Path();
	int nrOfNodes = 0;
	returnPath->nrOfNodes = 0;
	_currentMapNode = targetMapNode;
	while (_currentMapNode->ID != startPosition)
	{
		nrOfNodes++;
		_currentMapNode = _mapGrid[_currentMapNode->parentMapNode];
	}
	returnPath->nrOfNodes = nrOfNodes;
	returnPath->nodes = new MapGridPairs[nrOfNodes+1];
	_currentMapNode = targetMapNode;
	while (_currentMapNode->ID != startPosition)
	{
		returnPath->nodes[nrOfNodes] = _currentMapNode->position;
		nrOfNodes--;
		_currentMapNode = _mapGrid[_currentMapNode->parentMapNode];
	}

	returnPath->nodes[0] = _currentMapNode->position;

	return returnPath;

	//Starta i en punkt
	//L�gg in alla noder runt omkring i en "�ppen" lista.
	//L�gg in startpunkten i en "st�ngd" lista.
	//Evaluera G v�rdet p� alla punkter runt omkring.
	//Hitta det minsta F v�rdet (h +g), avst�ndet fr�n noden till m�let, + kostnaden f�r att flytta till den noden.
	//Flytta till denna nod, och l�gg till den i closed list, ta bort den fr�n open list.
	//L�gg punkterna runt den i den �ppna listan.
	//Ber�kna G v�rdet p� alla punkterna runt omkring, om den inte ligger i Closed List.
	//Om G v�rdet fr�n en nod �r mindre �n det var tidigare, �ndra till det nya v�rdet och l�gg "current node" till "ParentMapNode" (s�tt dess ID som "parentMapNode").
	//G� igenom listan f�r att hitta det l�gsta F v�rdet, och v�lj det som n�sta "current".
	//Forts�tt tills vi kommer till en node som sitter j�mte v�r node. 
	//G�r en "backtracking" path, fr�n target till start.
}

void VeryBasicAI::clear()
{
	for (int i = 0; i < _size; i++)
	{
		_mapGrid[i]->gValue = INT_MAX;
		_mapGrid[i]->fValue = INT_MAX;
		_mapGrid[i]->parentMapNode = i;
		_mapGrid[i]->walkHere = 0;
		_openList[i] = 0;
		_closedList[i] = 0;
	}
	for (int i = 0; i < _sizeOfOpenList; i++)
	{
		_openNodeList[i] = nullptr;
	}
	_sizeOfOpenList = 0;
}