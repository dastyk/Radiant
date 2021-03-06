#ifndef _DUNGEON_H_
#define _DUNGEON_H_

#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <d3d.h>
#include <DirectXMath.h>
#include "General.h"


#define COLLISSIONX 1
#define COLLISSIONY 2

#include "Entity.h"
class EntityBuilder;

struct room
{
	int posWidth;
	int posHeight;

	int extWidth;
	int extHeight;

	room()
	{
		posWidth = 0;
		posHeight = 0;
		extWidth = 0;
		extHeight = 0;
	}

	room(int widthPos, int heightPos, int extentX, int extentY)
	{
		posWidth = widthPos;
		posHeight = heightPos;
		extWidth = extentX;
		extHeight = extentY;
	}
};

struct FreePositions
{
	FreePositions(int posX, int posY)
	{
		x = posX;
		y = posY;
	}

	int x;
	int y;
};

class Dungeon
{
private:

	int** tiles;
	room* rooms;
	std::vector<FreePositions> freePositions;
	std::vector<FreePositions> freeRoomPositions;
	std::vector<int> occupied;
	int DungeonWidth;
	int DungeonHeight;

	float percentCovered;
	float percentToCover;
	int minimumExtent;
	int maximumExtent;
	int nrOfRooms;
	
	std::vector<SubMeshInfo> wallInfo;
	std::vector<DirectX::XMFLOAT3> positionVector;
	std::vector<DirectX::XMFLOAT2> uvVector;
	std::vector<unsigned int> indicesVector;
	std::vector<Entity> walls;
	std::vector<Entity> floorroof;
	std::vector<Entity> pillars;
	EntityBuilder* _builder;

	void generateCorridors();
	void setRoomTiles();
	void setRoomTiles(int roomNr);

	void CreateWallsUTD(std::vector<DirectX::XMFLOAT3> &positions, std::vector<DirectX::XMFLOAT2> &uv, std::vector<unsigned int> &indices, std::vector<int> startPos, std::vector<int> lengths, int offset);
	void CreateWallsLTR(std::vector<DirectX::XMFLOAT3> &positions, std::vector<DirectX::XMFLOAT2> &uv, std::vector<unsigned int> &indices, std::vector<int> startPos, std::vector<int> lengths, int offset);

	void generateDungeonRecursive();
	void removeWalls();
	void GenerateGraphicalData(unsigned int level);



public:
	Dungeon(int width, int height, EntityBuilder* builder);
	Dungeon(int side, int minimumExtent, int maximumExtent, float percentToCover, EntityBuilder* builder, unsigned int level);
	virtual ~Dungeon();

	void generateDungeon(unsigned int level);
	int checkRoomCollision(room room1, room room2);

	int getTile(int widthPos, int heightPos);
	std::vector<SubMeshInfo> GetSubMeshInfo();

	std::vector<DirectX::XMFLOAT3>& GetPosVector();
	std::vector<DirectX::XMFLOAT2>& GetUvVector();
	std::vector<unsigned int>& GetIndicesVector();

	const std::vector<FreePositions>& GetFreePositions();
	const std::vector<FreePositions>& GetFreeRoomPositions();

	const std::vector<Entity>& GetWalls()const;
	const std::vector<Entity>& GetFloorRoof()const;
	const std::vector<Entity>& GetPillars()const;
	const FreePositions GetunoccupiedSpace();
};

#endif