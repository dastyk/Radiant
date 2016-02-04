

#ifndef _DUNGEON_H_
#define _DUNGEON_H_

#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <d3d.h>
#include <DirectXMath.h>


#define COLLISSIONX 1
#define COLLISSIONY 2

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

class Dungeon
{
private:

	int** tiles;
	room* rooms;

	int DungeonWidth;
	int DungeonHeight;

	float percentCovered;
	int minimumExtent;
	int maximumExtent;
	int nrOfRooms;

	std::vector<DirectX::XMFLOAT3> positionVector;
	std::vector<DirectX::XMFLOAT2> uvVector;
	std::vector<unsigned int> indicesVector;

	void generateCorridors();
	void setRoomTiles();
	void setRoomTiles(int roomNr);

	void CreateWallsUTD(std::vector<DirectX::XMFLOAT3> &positions, std::vector<DirectX::XMFLOAT2> &uv, std::vector<unsigned int> &indices, std::vector<int> startPos, std::vector<int> lengths, int offset);
	void CreateWallsLTR(std::vector<DirectX::XMFLOAT3> &positions, std::vector<DirectX::XMFLOAT2> &uv, std::vector<unsigned int> &indices, std::vector<int> startPos, std::vector<int> lengths, int offset);

	void removeWalls();
	void GenerateGraphicalData();

public:
	Dungeon(int width, int height);
	virtual ~Dungeon();

	void generateDungeon();
	int checkRoomCollision(room room1, room room2);

	int getTile(int widthPos, int heightPos);

	std::vector<DirectX::XMFLOAT3>& GetPosVector();
	std::vector<DirectX::XMFLOAT2>& GetUvVector();
	std::vector<unsigned int>& GetIndicesVector();

};

#endif