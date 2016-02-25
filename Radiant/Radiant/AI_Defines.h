#ifndef AI_DEFINES_H
#define AI_DEFINES_H

#include "General.h"
#include "Utils.h"


#define timeBeforeRecalculatingMovementVector 0.2f
#define hValueMultiplier 3
#define diagonalMoveCost 280
#define straightMoveCost 20
#define timeBeforePlayerVanishes 1.25f
#define enemySightRadius 5.0f
#define goldenAngel		DirectX::XM_PIDIV4
#define wallOffsetDiagonal 0.0625f
#define wallOffsetOther 0.125f

struct MapGridPairs
{
	float x = -1.0f, y = -1.0f;
	float offsetX = 0.0f, offsetY = 0.0f;

	MapGridPairs()
	{
		this->x = this->y = -1.0f;
		this->offsetX = this->offsetY = 0.0f;
	}

	MapGridPairs(float positionX, float positionY, float offsetX = 0.0f, float offsetY = 0.0f)
	{
		this->x = positionX;
		this->y = positionY;
		this->offsetX = offsetX;
		this->offsetY = offsetY;
	}
};

struct Path
{
	int nrOfNodes;
	Path() : nodes(nullptr)
	{}
	~Path()
	{
		SAFE_DELETE_ARRAY(nodes);
	}
	MapGridPairs* nodes = nullptr;
};

struct MapNode
{
	int parentMapNode = -1;
	MapGridPairs position;
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
