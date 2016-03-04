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
	void _calculateHCost();
	bool _evaluateAroundMapNode();
	void _initializeClosedList();
	void _sortOpenList();

	bool* _openList = nullptr;
	MapNode** _openNodeList = nullptr;
	bool* _closedList = nullptr;
	int _sizeOfOpenList;
	MapNode** _mapGrid = nullptr;
	int _size;
	MapNode* _currentMapNode = nullptr;
	MapNode* _nodeToBeEvaluated = nullptr;
	MapNode* _targetMapNode = nullptr;
	int _maxSize;
	int _counter = 0;
};


#endif // !_VERY_BASIC_AI_H_
