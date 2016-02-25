#include "VeryBasicAI.h"


VeryBasicAI::VeryBasicAI(MapNode** mapGrid, int size)
{
	this->size = size;
	this->maxSize = (int)sqrt(size);
	this->mapGrid = new MapNode*[size];
	for (int i = 0; i < size; i++)
	{
		this->mapGrid[i] = mapGrid[i];
		if (this->mapGrid[i]->type == 0)
		{
			if ((i%maxSize) != 0)
			{
				if (mapGrid[i - 1]->type != 0)
				{
					mapGrid[i]->position.offsetX += wallOffsetOther;
				}
			}
			if ((i + 1) % maxSize != 0)
			{
				if (mapGrid[i + 1]->type != 0)
				{
					mapGrid[i]->position.offsetX -= wallOffsetOther;
				}
			}
			if (!((i + maxSize) > size))
			{
				if (mapGrid[i + maxSize]->type != 0)
				{
					mapGrid[i]->position.offsetY -= wallOffsetOther;
				}
				if (!((i) % maxSize != 0))
				{
					if (mapGrid[i + maxSize - 1]->type != 0)
					{
						mapGrid[i]->position.offsetY -= wallOffsetDiagonal;
						mapGrid[i]->position.offsetX += wallOffsetDiagonal;
					}
				}
				if (!((i + 1) % maxSize != 0))
				{
					if (!(i + maxSize + 1 > size))
					{
						if (mapGrid[i + maxSize + 1]->type != 0)
						{
							mapGrid[i]->position.offsetY -= wallOffsetDiagonal;
							mapGrid[i]->position.offsetX -= wallOffsetDiagonal;
						}
					}
				}
			}
		}
		if (!((i - maxSize) < 0))
		{
			if (mapGrid[i - maxSize]->type != 0)
			{
				mapGrid[i]->position.offsetY += wallOffsetOther;
			}
			if (!(i + 1) % maxSize != 0)
			{
				if (mapGrid[i - maxSize + 1]->type != 0)
				{
					mapGrid[i]->position.offsetX -= wallOffsetDiagonal;
					mapGrid[i]->position.offsetY += wallOffsetDiagonal;
				}
			}
			if (!(i) % maxSize != 0)
			{
				if (!(i - maxSize - 1 < 0))
				{
					if (mapGrid[i - maxSize - 1]->type != 0)
					{
						mapGrid[i]->position.offsetX += wallOffsetDiagonal;
						mapGrid[i]->position.offsetY += wallOffsetDiagonal;
					}
				}
			}
		}
	}
	closedList = new bool[size]; //Worst case scenario (can't happen)
	openList = new bool[size]; //Same as above.
	openNodeList = new MapNode*[size];
	sizeOfOpenList = 0;

}

VeryBasicAI::~VeryBasicAI()
{
	delete[] closedList;
	delete[] openList;
	delete[] mapGrid;
	delete[] openNodeList;
}

void VeryBasicAI::calculateHCost()	//Size is the size of the entire MapNode array, that is size*size of the room array.
{
	for (int i = 0; i < size; i++)
	{
		mapGrid[i]->hValue = 0;

		if (mapGrid[i]->type != 0)
		{
			mapGrid[i]->hValue = -1;
		}
		else if (mapGrid[i]->ID == targetMapNode->ID)
		{

		}
		else if (mapGrid[i]->position.x < targetMapNode->position.x)
		{
			mapGrid[i]->hValue += targetMapNode->position.x - mapGrid[i]->position.x;
			if (mapGrid[i]->position.y < targetMapNode->position.y)
			{
				mapGrid[i]->hValue += (targetMapNode->position.y - mapGrid[i]->position.y)*hValueMultiplier;
			}
			else
			{
				mapGrid[i]->hValue += mapGrid[i]->position.y - targetMapNode->position.y*hValueMultiplier;
			}
		}
		else
		{
			mapGrid[i]->hValue += mapGrid[i]->position.x - targetMapNode->position.x;
			if (mapGrid[i]->position.y < targetMapNode->position.y)
			{
				mapGrid[i]->hValue += targetMapNode->position.y - mapGrid[i]->position.y*hValueMultiplier;
			}
			else
			{
				mapGrid[i]->hValue += mapGrid[i]->position.y - targetMapNode->position.y*hValueMultiplier;
			}
		}
	}
}


bool VeryBasicAI::evaluateAroundMapNode()
{
	//When we evaluate a node, we check the nodes around it, to find the "cheapest" way.
	//This is done by moving the "nodeToBeEvaluated", check if this node is closed (already checked, a wall or void) and, if not, check if it has been evaluated previously.
	//If it has not been evaluated previously, or if it's evaluated G value is lower than the G value we get by going through thé currentMapNode, we set the gValue to either
	//currentMapNode->gValue + 10, if the way to the point is straight, or currentMapNode->gValue + 14, if diagonal.

	//We start with the MapNode above
	if (currentMapNode->ID - maxSize > 0)
	{
		nodeToBeEvaluated = mapGrid[currentMapNode->ID - maxSize];
		if (nodeToBeEvaluated == targetMapNode)
		{
			mapGrid[targetMapNode->ID]->parentMapNode = currentMapNode->ID;
			return true;
		}
		if (!closedList[nodeToBeEvaluated->ID])
		{
			if (!openList[nodeToBeEvaluated->ID])
			{
				nodeToBeEvaluated->gValue = currentMapNode->gValue + straightMoveCost;
				nodeToBeEvaluated->calculateF();
				nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
				openList[nodeToBeEvaluated->ID] = 1;
				openNodeList[sizeOfOpenList] = nodeToBeEvaluated;
				sizeOfOpenList++;

			}
			else if (nodeToBeEvaluated->gValue > currentMapNode->gValue + straightMoveCost)
			{
				nodeToBeEvaluated->gValue = currentMapNode->gValue + straightMoveCost;
				nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
				nodeToBeEvaluated->calculateF();
			}
		}
		//MapNode above to the left
		if ((currentMapNode->ID - maxSize) % maxSize)
		{
			nodeToBeEvaluated = mapGrid[currentMapNode->ID - maxSize - 1];
			if (nodeToBeEvaluated == targetMapNode)
			{
				mapGrid[targetMapNode->ID]->parentMapNode = currentMapNode->ID;
				return true;
			}
			if (!closedList[nodeToBeEvaluated->ID])
			{
				if (!openList[nodeToBeEvaluated->ID])
				{
					nodeToBeEvaluated->gValue = currentMapNode->gValue + diagonalMoveCost;
					nodeToBeEvaluated->calculateF();
					nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
					openList[nodeToBeEvaluated->ID] = 1;
					openNodeList[sizeOfOpenList] = nodeToBeEvaluated;
					sizeOfOpenList++;

				}
				else if (nodeToBeEvaluated->gValue > currentMapNode->gValue + diagonalMoveCost)
				{
					nodeToBeEvaluated->gValue = currentMapNode->gValue + diagonalMoveCost;
					nodeToBeEvaluated->calculateF();
					nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
				}
			}
		}
		//MapNode above to the right
		if ((currentMapNode->ID - maxSize + 1) % maxSize)
		{
			nodeToBeEvaluated = mapGrid[currentMapNode->ID - maxSize + 1];
			if (nodeToBeEvaluated == targetMapNode)
			{
				mapGrid[targetMapNode->ID]->parentMapNode = currentMapNode->ID;
				return true;
			}
			if (!closedList[nodeToBeEvaluated->ID])
			{
				if (!openList[nodeToBeEvaluated->ID])
				{
					nodeToBeEvaluated->gValue = currentMapNode->gValue + diagonalMoveCost;
					nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
					nodeToBeEvaluated->calculateF();
					openList[nodeToBeEvaluated->ID] = 1;
					openNodeList[sizeOfOpenList] = nodeToBeEvaluated;
					sizeOfOpenList++;
				}
				else if (nodeToBeEvaluated->gValue > currentMapNode->gValue + diagonalMoveCost)
				{
					nodeToBeEvaluated->gValue = currentMapNode->gValue + diagonalMoveCost;
					nodeToBeEvaluated->calculateF();
					nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
				}
			}
		}
	}

	//MapNode below
	if (currentMapNode->ID < size - maxSize)
	{
		nodeToBeEvaluated = mapGrid[currentMapNode->ID + maxSize];
		if (nodeToBeEvaluated == targetMapNode)
		{
			mapGrid[targetMapNode->ID]->parentMapNode = currentMapNode->ID;
			return true;
		}
		if (!closedList[nodeToBeEvaluated->ID])
		{
			if (!openList[nodeToBeEvaluated->ID])
			{
				nodeToBeEvaluated->gValue = currentMapNode->gValue + straightMoveCost;
				nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
				nodeToBeEvaluated->calculateF();
				openList[nodeToBeEvaluated->ID] = 1;
				openNodeList[sizeOfOpenList] = nodeToBeEvaluated;
				sizeOfOpenList++;
			}
			else if (nodeToBeEvaluated->gValue > currentMapNode->gValue + straightMoveCost)
			{
				mapGrid[currentMapNode->ID + maxSize]->gValue = currentMapNode->gValue + straightMoveCost;
				nodeToBeEvaluated->calculateF();
				nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
			}
		}
		//MapNode below to the left
		if ((currentMapNode->ID + maxSize) % maxSize)
		{
			nodeToBeEvaluated = mapGrid[currentMapNode->ID + maxSize - 1];
			if (nodeToBeEvaluated == targetMapNode)
			{
				mapGrid[targetMapNode->ID]->parentMapNode = currentMapNode->ID;
				return true;
			}
			if (!closedList[nodeToBeEvaluated->ID])
			{
				if (!openList[nodeToBeEvaluated->ID])
				{
					nodeToBeEvaluated->gValue = currentMapNode->gValue + diagonalMoveCost;
					nodeToBeEvaluated->calculateF();
					nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
					openList[nodeToBeEvaluated->ID] = 1;
					openNodeList[sizeOfOpenList] = nodeToBeEvaluated;
					sizeOfOpenList++;
				}
				else if (nodeToBeEvaluated->gValue > currentMapNode->gValue + diagonalMoveCost)
				{
					nodeToBeEvaluated->gValue = currentMapNode->gValue + diagonalMoveCost;
					nodeToBeEvaluated->calculateF();
					nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
				}
			}
		}

		//MapNode below to the right
		if ((currentMapNode->ID + maxSize + 1) % maxSize)
		{
			nodeToBeEvaluated = mapGrid[currentMapNode->ID + maxSize + 1];
			if (nodeToBeEvaluated == targetMapNode)
			{
				mapGrid[targetMapNode->ID]->parentMapNode = currentMapNode->ID;
				return true;
			}

			if (!closedList[nodeToBeEvaluated->ID])
			{
				if (!openList[nodeToBeEvaluated->ID])
				{
					nodeToBeEvaluated->gValue = currentMapNode->gValue + diagonalMoveCost;
					nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
					nodeToBeEvaluated->calculateF();
					openList[nodeToBeEvaluated->ID] = 1;
					openNodeList[sizeOfOpenList] = nodeToBeEvaluated;
					sizeOfOpenList++;

				}
				else if (nodeToBeEvaluated->gValue > currentMapNode->gValue + diagonalMoveCost)
				{
					nodeToBeEvaluated->gValue = currentMapNode->gValue + diagonalMoveCost;
					nodeToBeEvaluated->calculateF();
					nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
				}
			}
		}
	}

	//MapNode to the right
	if ((currentMapNode->ID + 1) % maxSize)
	{
		nodeToBeEvaluated = mapGrid[currentMapNode->ID + 1];
		if (nodeToBeEvaluated == targetMapNode)
		{
			mapGrid[targetMapNode->ID]->parentMapNode = currentMapNode->ID;
			return true;
		}

		if (!closedList[nodeToBeEvaluated->ID])
		{
			if (!openList[nodeToBeEvaluated->ID])
			{
				nodeToBeEvaluated->gValue = currentMapNode->gValue + straightMoveCost;
				nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
				nodeToBeEvaluated->calculateF();
				openList[nodeToBeEvaluated->ID] = 1;
				openNodeList[sizeOfOpenList] = nodeToBeEvaluated;
				sizeOfOpenList++;
			}
			else if (nodeToBeEvaluated->gValue > currentMapNode->gValue + straightMoveCost)
			{
				nodeToBeEvaluated->gValue = currentMapNode->gValue + straightMoveCost;
				nodeToBeEvaluated->calculateF();
				nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
			}
		}
	}

	//MapNode to the left
	if ((currentMapNode->ID) % maxSize)
	{
		nodeToBeEvaluated = mapGrid[currentMapNode->ID - 1];
		if (nodeToBeEvaluated == targetMapNode)
		{
			mapGrid[targetMapNode->ID]->parentMapNode = currentMapNode->ID;
			nodeToBeEvaluated->calculateF();
			return true;
		}

		if (!closedList[nodeToBeEvaluated->ID])
		{
			if (!openList[nodeToBeEvaluated->ID])
			{
				nodeToBeEvaluated->gValue = currentMapNode->gValue + straightMoveCost;
				nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
				nodeToBeEvaluated->calculateF();
				openList[nodeToBeEvaluated->ID] = 1;
				openNodeList[sizeOfOpenList] = nodeToBeEvaluated;
				sizeOfOpenList++;
			}
			else if (nodeToBeEvaluated->gValue > currentMapNode->gValue + straightMoveCost)
			{
				nodeToBeEvaluated->gValue = currentMapNode->gValue + straightMoveCost;
				nodeToBeEvaluated->calculateF();
				nodeToBeEvaluated->parentMapNode = currentMapNode->ID;
			}
		}
	}

	return false;
}

//Function to add all walls and void to the closed, so they're never evaluated. Returns the number of closed objects.
//Not that the function takes for granted that the "closedList" array is empty.
void VeryBasicAI::initializeClosedList()
{
	for (int i = 0; i < size; i++)
	{
		if (mapGrid[i]->type)
		{
			closedList[i] = 1;
		}
		else
		{
			closedList[i] = 0;
		}

	}

}

void VeryBasicAI::sortOpenList()
{
	MapNode* temp;
	int position;
	temp = openNodeList[sizeOfOpenList - 1];
	position = sizeOfOpenList - 1;
	for (int i = sizeOfOpenList - 2; i > 0; i--)
	{
		if (openNodeList[i]->fValue < temp->fValue)
		{
			temp = openNodeList[i];
			position = i;
		}
	}
	openNodeList[position] = openNodeList[sizeOfOpenList - 1];
	openNodeList[sizeOfOpenList - 1] = temp;

}

//Returnera en "lista" över punkter att röra sig till, samt hur många punkter som skall röras sig igenom.
Path* VeryBasicAI::basicAStar(int startPosition, MapNode *targetMapNode)
{
	
	clear();
	initializeClosedList();
	if (closedList)
	{
		if (closedList[startPosition])
			return nullptr;
	}
	else
		return nullptr;
	if (closedList[targetMapNode->ID])
		return nullptr;
	this->targetMapNode = targetMapNode;
	calculateHCost();

	currentMapNode = mapGrid[startPosition];
	currentMapNode->gValue = 0;
	closedList[currentMapNode->ID] = 1;

	while (!evaluateAroundMapNode())
	{
		if (sizeOfOpenList <= 0)
		{
			return nullptr;
		}
		sortOpenList();
		sizeOfOpenList--;
		currentMapNode = openNodeList[sizeOfOpenList];
		closedList[currentMapNode->ID] = 1;
		openList[currentMapNode->ID] = 0;
	}
	Path* returnPath;
	returnPath = new Path();
	int nrOfNodes = 0;
	returnPath->nrOfNodes = 0;
	currentMapNode = targetMapNode;
	while (currentMapNode->ID != startPosition)
	{
		nrOfNodes++;
		currentMapNode = mapGrid[currentMapNode->parentMapNode];
	}
	returnPath->nrOfNodes = nrOfNodes;
	returnPath->nodes = new MapGridPairs[nrOfNodes+1];
	currentMapNode = targetMapNode;
	while (currentMapNode->ID != startPosition)
	{
		returnPath->nodes[nrOfNodes] = currentMapNode->position;
		nrOfNodes--;
		currentMapNode = mapGrid[currentMapNode->parentMapNode];
	}

	returnPath->nodes[0] = currentMapNode->position;

	return returnPath;

	//Starta i en punkt
	//Lägg in alla noder runt omkring i en "Öppen" lista.
	//Lägg in startpunkten i en "stängd" lista.
	//Evaluera G värdet på alla punkter runt omkring.
	//Hitta det minsta F värdet (h +g), avståndet från noden till målet, + kostnaden för att flytta till den noden.
	//Flytta till denna nod, och lägg till den i closed list, ta bort den från open list.
	//Lägg punkterna runt den i den öppna listan.
	//Beräkna G värdet på alla punkterna runt omkring, om den inte ligger i Closed List.
	//Om G värdet från en nod är mindre än det var tidigare, ändra till det nya värdet och lägg "current node" till "ParentMapNode" (sätt dess ID som "parentMapNode").
	//Gå igenom listan för att hitta det lägsta F värdet, och välj det som nästa "current".
	//Fortsätt tills vi kommer till en node som sitter jämte vår node. 
	//Gör en "backtracking" path, från target till start.
}

void VeryBasicAI::clear()
{
	for (int i = 0; i < size; i++)
	{
		mapGrid[i]->gValue = INT_MAX;
		mapGrid[i]->fValue = INT_MAX;
		mapGrid[i]->parentMapNode = i;
		mapGrid[i]->walkHere = 0;
		openList[i] = 0;
		closedList[i] = 0;
	}
	for (int i = 0; i < sizeOfOpenList; i++)
	{
		openNodeList[i] = nullptr;
	}
	sizeOfOpenList = 0;
}