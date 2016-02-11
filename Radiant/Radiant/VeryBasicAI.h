#ifndef _VERY_BASIC_AI_H_
#define _VERY_BASIC_AI_H_

#include "AI_Defines.h"

class VeryBasicAI
{
public:
	Path* basicAStar(int startPosition, MapNode *targetMapNode);
	void clear();
	VeryBasicAI(MapNode** mapGrid, int size);
	~VeryBasicAI();

private:
	void calculateHCost();
	bool evaluateAroundMapNode();
	void initializeClosedList();
	void sortOpenList();

	bool* openList;
	MapNode** openNodeList;
	bool* closedList;
	int sizeOfOpenList;
	MapNode** mapGrid;
	int size;
	MapNode* currentMapNode;
	MapNode* nodeToBeEvaluated;
	MapNode* targetMapNode;
	int maxSize;
	int counter = 0;
};


#endif // !_VERY_BASIC_AI_H_
