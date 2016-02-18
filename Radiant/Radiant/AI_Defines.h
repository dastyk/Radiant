#ifndef AI_DEFINES_H
#define AI_DEFINES_H

#include "General.h"
#include "Utils.h"

#define hValueMultiplier 3
#define diagonalMoveCost 28
#define straightMoveCost 20

struct Pair
{
	int x = -1, y = -1;
};

struct Path
{
	int nrOfNodes;
	Pair* nodes = nullptr;
	Path() : nodes(nullptr)
	{}
	~Path()
	{
		SAFE_DELETE_ARRAY(nodes);
	}
};

struct MapNode
{
	int parentMapNode = -1;
	Pair position;
	int ID = -1;
	int type = -1;	//Floor, wall or empty.

	int hValue = INT_MAX;
	int gValue = INT_MAX;
	int fValue = INT_MAX;

	int walkHere = 0;

	void calculateF()
	{
		fValue = gValue + hValue;
	}
	MapNode* operator=(MapNode &original)
	{
		this->parentMapNode = original.parentMapNode;
		this->position = original.position;
		this->ID = original.ID;
		this->type = original.type;
		this->hValue = original.hValue;
		this->gValue = original.gValue;
		this->fValue = original.fValue;

		return this;
	}
	bool operator<(MapNode& original)
	{
		if (this->fValue < original.fValue)
		{
			return true;
		}
		return false;
	}
	bool operator>(MapNode& original)
	{
		if (this->fValue > original.fValue)
		{
			return true;
		}
		return false;
	}
	bool operator>=(MapNode& original)
	{
		if (this->fValue >= original.fValue)
		{
			return true;
		}
		return false;
	}
	bool operator<=(MapNode& original)
	{
		if (this->fValue <= original.fValue)
		{
			return true;
		}
		return false;
	}
};




#endif
