#include "Dungeon.h"
using namespace DirectX;
using namespace std;

Dungeon::Dungeon(int width, int height)
{
	tiles = new int*[width];
	srand(static_cast<unsigned int>(time(NULL)));

	for (int i = 0; i < width; i++)
	{
		tiles[i] = new int[height];
	}

	DungeonWidth = width;
	DungeonHeight = height;

	percentCovered = 1;
	percentToCover = 0.75;
	minimumExtent = 2;
	maximumExtent = 4;
	nrOfRooms = 0;

	rooms = new room[(DungeonWidth * DungeonHeight) / (minimumExtent * minimumExtent)];

	generateDungeon();
}

Dungeon::Dungeon(int side, int minimumExtent, int maximumExtent, float percentToCover)
{
	tiles = new int*[side];
	srand(static_cast<unsigned int>(time(NULL)));

	for (int i = 0; i < side; i++)
	{
		tiles[i] = new int[side];
	}

	DungeonWidth = side;
	DungeonHeight = side;

	percentCovered = 1;
	this->percentToCover = percentToCover;
	this->minimumExtent = minimumExtent;
	this->maximumExtent = maximumExtent; // make larger when done
	nrOfRooms = 0;

	rooms = new room[(DungeonWidth * DungeonHeight) / (minimumExtent * minimumExtent)];

	generateDungeon();
}

Dungeon::~Dungeon()
{
	for (int i = 0; i < DungeonWidth; i++)
	{
		delete tiles[i];
	}

	delete[] tiles;

	delete rooms;
}

void Dungeon::generateDungeon()
{
	int widthPos;
	int heightPos;
	int extentX;
	int extentY;
	bool fits;
	int nrOfTries = 0;

	for (int i = 0; i < DungeonWidth; i++)
	{
		for (int j = 0; j < DungeonHeight; j++)
		{
			tiles[i][j] = 1;
		}
	}

	nrOfRooms = 0;

	while (percentCovered > percentToCover) // CHANGE TO 0.99 WHEN TESTING
	{
		fits = true;

		extentX = (rand() % (maximumExtent - minimumExtent + 1)) + minimumExtent;
		extentY = (rand() % (maximumExtent - minimumExtent + 1)) + minimumExtent;
		widthPos = rand() % (DungeonWidth - extentX * 2 - 2) + extentX + 1;
		heightPos = rand() % (DungeonHeight - extentY * 2 - 2) + extentY + 1;
		Sleep(10);

		for (int i = 0; i < nrOfRooms; i++)
		{
			int temp = checkRoomCollision(rooms[i], room(widthPos, heightPos, extentX, extentY));

			if (temp > 0)
			{
				if (extentX <= minimumExtent || extentY <= minimumExtent)
				{
					fits = false;
					i = nrOfRooms;
					nrOfTries++;
					if (nrOfTries > 30)
					{
						generateDungeon();
					}
				}
				else if (temp == COLLISSIONX)
				{
					extentX--;
					i -= 1;
				}
				else
				{
					extentY--;
					i -= 1;
				}
			}
		}

		if (fits == true)
		{
			nrOfTries = 0;
			rooms[nrOfRooms].posWidth = widthPos;
			rooms[nrOfRooms].posHeight = heightPos;
			rooms[nrOfRooms].extWidth = extentX;
			rooms[nrOfRooms].extHeight = extentY;

			nrOfRooms++;

			setRoomTiles(nrOfRooms - 1);

			int occupiedTiles = 0;

			for (int i = 0; i < DungeonWidth; i++)
			{
				for (int j = 0; j < DungeonHeight; j++)
				{
					if (tiles[i][j] == 0)
					{
						occupiedTiles += 1;
					}
				}
			}

			percentCovered = 1 - (float)occupiedTiles / (DungeonWidth * DungeonHeight);

		}
	}

	generateCorridors();
	removeWalls();
	GenerateGraphicalData();

}

int Dungeon::checkRoomCollision(room room1, room room2)
{
	if (abs(room1.posWidth - room2.posWidth) < minimumExtent + 1)
	{
		return COLLISSIONX;
	}

	if (abs(room1.posHeight - room2.posHeight) < minimumExtent + 1)
	{
		return COLLISSIONY;
	}

	return 0;

}

void Dungeon::generateCorridors()
{
	for (int i = 0; i < nrOfRooms; i++)
	{
		int maxDistance = 0; // corridors in x (horizontally)
		int minDistance = 0;
		int temp;

		for (int j = 0; j < nrOfRooms; j++)
		{
			temp = rooms[j].posWidth - rooms[i].posWidth;

			if (temp > maxDistance)
			{
				maxDistance = temp;
			}
			else if (temp < minDistance)
			{
				minDistance = temp;
			}
		}

		for (int j = 0; j <= maxDistance; j++)
		{
			tiles[rooms[i].posWidth + j][rooms[i].posHeight] = 0;
			tiles[rooms[i].posWidth + j][rooms[i].posHeight - 1] = 0;
		}

		for (int j = 0; j >= minDistance; j--)
		{
			tiles[rooms[i].posWidth + j][rooms[i].posHeight] = 0;
			tiles[rooms[i].posWidth + j][rooms[i].posHeight - 1] = 0;
		}


		maxDistance = 0; // corridors in y (vertically)
		minDistance = 0;

		for (int j = 0; j < nrOfRooms; j++)
		{
			temp = rooms[j].posHeight - rooms[i].posHeight;

			if (temp > maxDistance)
			{
				maxDistance = temp;
			}
			else if (temp < minDistance)
			{
				minDistance = temp;
			}
		}

		for (int j = 0; j <= maxDistance; j++)
		{
			tiles[rooms[i].posWidth][rooms[i].posHeight + j] = 0;
			tiles[rooms[i].posWidth - 1][rooms[i].posHeight + j] = 0;
		}


		for (int j = 0; j >= minDistance; j--)
		{
			tiles[rooms[i].posWidth][rooms[i].posHeight + j] = 0;
			tiles[rooms[i].posWidth - 1][rooms[i].posHeight + j] = 0;
		}

	}
}

void Dungeon::setRoomTiles()
{
	for (int i = 0; i < nrOfRooms; i++)
	{
		for (int j = 0; j < rooms[i].extWidth * 2 + 1; j++)
		{
			for (int k = 0; k < rooms[i].extHeight * 2 + 1; k++)
			{
				tiles[rooms[i].posWidth - rooms[i].extWidth + j][rooms[i].posHeight - rooms[i].extHeight + k] = 0;
			}
		}
	}
}

void Dungeon::setRoomTiles(int roomNr)
{
	for (int i = 0; i < rooms[roomNr].extWidth * 2 + 1; i++)
	{
		for (int j = 0; j < rooms[roomNr].extHeight * 2 + 1; j++)
		{
			tiles[rooms[roomNr].posWidth - rooms[roomNr].extWidth + i][rooms[roomNr].posHeight - rooms[roomNr].extHeight + j] = 0;
		}
	}
}

void Dungeon::removeWalls()
{
	for (int i = 1; i < DungeonWidth - 1; i++)
	{
		for (int j = 1; j < DungeonHeight - 1; j++)
		{
			if (tiles[i - 1][j] == 0 || tiles[i + 1][j] == 0 || tiles[i][j - 1] == 0 || tiles[i][j + 1] == 0)
			{
				// Do nothing
			}
			else
			{
				tiles[i][j] = 2;
			}
		}
	}

	for (int i = 0; i < DungeonWidth; i++)
	{
		if (tiles[i][1] == 0)
		{
			// Do nothing
		}
		else
		{
			tiles[i][0] = 2;
		}

		if (tiles[i][DungeonHeight - 2] == 0)
		{
			// Do nothing
		}
		else
		{
			tiles[i][DungeonHeight - 1] = 2;
		}
	}

	for (int i = 0; i < DungeonHeight; i++)
	{
		if (tiles[1][i] == 0)
		{
			// Do nothing
		}
		else
		{
			tiles[0][i] = 2;
		}

		if (tiles[DungeonWidth - 2][i] == 0)
		{
			// Do nothing
		}
		else
		{
			tiles[DungeonWidth - 1][i] = 2;
		}
	}
}

void Dungeon::GenerateGraphicalData()
{
	int nrOfWallsFound = 0;
	int wallSections = 0;

	vector<int> startPosUTD;
	vector<int> lengthUTD;

	vector<int> startPosLTR;
	vector<int> lengthLTR;

	int** takenTiles;
	takenTiles = new int*[DungeonWidth];

	for (int i = 0; i < DungeonWidth; i++) // Generate pre buffer
	{
		takenTiles[i] = new int[DungeonHeight];

		for (int j = 0; j < DungeonHeight; j++)
		{
			if (tiles[i][j] == 1)
			{
				takenTiles[i][j] = 0;
			}
			else
			{
				takenTiles[i][j] = -1;
			}
		}
	}

	for (int i = 0; i < DungeonWidth; i++) // go through from up to down
	{
		for (int j = 1; j < DungeonHeight; j++)
		{
			if (tiles[i][j] == 1 && takenTiles[i][j] != 1)
			{
				nrOfWallsFound += 1;

				if (nrOfWallsFound == 2)
				{
					startPosUTD.push_back(i);
					startPosUTD.push_back(j - 1);
				}
			}
			else
			{
				if (nrOfWallsFound > 1)
				{
					wallSections += 1;

					for (int k = 0; k < nrOfWallsFound; k++)
					{
						takenTiles[i][j - k - 1] = 1;
					}

					lengthUTD.push_back(nrOfWallsFound - 1);
				}

				nrOfWallsFound = 0;
			}
		}

		if (nrOfWallsFound > 1)
		{
			wallSections += 1;

			for (int k = 0; k < nrOfWallsFound; k++)
			{
				takenTiles[i][DungeonHeight - k - 1] = 1;
			}

			lengthUTD.push_back(nrOfWallsFound - 1);

		}

		nrOfWallsFound = 0;
	}

	nrOfWallsFound = 0;

	for (int i = 0; i < DungeonWidth; i++) // go through from left to right
	{
		for (int j = 1; j < DungeonHeight; j++)
		{
			if (tiles[j][i] == 1 && takenTiles[j][i] != 1)
			{
				nrOfWallsFound += 1;

				if (nrOfWallsFound == 2)
				{
					startPosLTR.push_back(j - 1);
					startPosLTR.push_back(i);
				}
			}
			else
			{
				if (nrOfWallsFound > 1)
				{
					wallSections += 1;

					for (int k = 0; k < nrOfWallsFound; k++)
					{
						takenTiles[j - k - 1][i] = 1;
					}

					lengthLTR.push_back(nrOfWallsFound - 1);

				}

				nrOfWallsFound = 0;
			}
		}

		if (nrOfWallsFound > 1)
		{
			wallSections += 1;

			for (int k = 0; k < nrOfWallsFound; k++)
			{
				takenTiles[DungeonWidth - k - 1][i] = 1;
			}

			lengthLTR.push_back(nrOfWallsFound - 1);

		}

		nrOfWallsFound = 0;
	}

	for (int i = 0; i < DungeonWidth; i++) // Check for single walls
	{
		for (int j = 1; j < DungeonHeight; j++)
		{
			if (takenTiles[i][j] == 0)
			{
				takenTiles[i][j] = 1;

				wallSections += 1;

				startPosUTD.push_back(i);
				startPosUTD.push_back(j - 1);

				lengthUTD.push_back(1);
			}
		}
	}

	for (int i = 0; i < DungeonWidth; i++)
	{
		delete takenTiles[i];
	}

	delete[] takenTiles;

	wallInfo.resize(lengthUTD.size() + lengthLTR.size() + 1);

	for (int i = 0; i < lengthUTD.size() + lengthLTR.size(); i++)
	{
		wallInfo[i].count = 24;
		wallInfo[i].indexStart = i * 24;
	}

	wallInfo[lengthUTD.size() + lengthLTR.size()].count = 6;
	wallInfo[lengthUTD.size() + lengthLTR.size()].indexStart = (lengthUTD.size() + lengthLTR.size()) * 24;

	vector<XMFLOAT3> tempPos;
	vector<XMFLOAT2> tempUv;
	vector<unsigned int> tempIndices;

	tempPos.resize(lengthUTD.size() * 16);
	tempUv.resize(lengthUTD.size() * 16);
	tempIndices.resize(lengthUTD.size() * 24);

	CreateWallsUTD(tempPos, tempUv, tempIndices, startPosUTD, lengthUTD, 0);

	positionVector.insert(positionVector.end(), tempPos.begin(), tempPos.end());
	uvVector.insert(uvVector.end(), tempUv.begin(), tempUv.end());
	indicesVector.insert(indicesVector.end(), tempIndices.begin(), tempIndices.end());


	tempPos.clear();
	tempUv.clear();
	tempIndices.clear();


	tempPos.resize(lengthLTR.size() * 16);
	tempUv.resize(lengthLTR.size() * 16);
	tempIndices.resize(lengthLTR.size() * 24);

	CreateWallsLTR(tempPos, tempUv, tempIndices, startPosLTR, lengthLTR, static_cast<int>(positionVector.size()));

	positionVector.insert(positionVector.end(), tempPos.begin(), tempPos.end());
	uvVector.insert(uvVector.end(), tempUv.begin(), tempUv.end());
	indicesVector.insert(indicesVector.end(), tempIndices.begin(), tempIndices.end());


	tempPos.clear();
	tempUv.clear();
	tempIndices.clear();

	tempPos.resize(4);
	tempUv.resize(4);
	tempIndices.resize(6);

	for (int i = 0; i < 4; i++) // floor
	{
		if (i == 0 || i == 2)
		{
			tempPos[i].x = 0;
			tempPos[i].y = 0;
			tempPos[i].z = DungeonHeight * (1 - i / 2);
		}
		else if (i == 1 || i == 3)
		{
			tempPos[i].x = DungeonWidth;
			tempPos[i].y = 0;
			tempPos[i].z = DungeonHeight * (1 - (i - 1) / 2);
		}
	}

	tempUv[0].x = 0;
	tempUv[0].y = DungeonHeight;

	tempUv[1].x = DungeonWidth;
	tempUv[1].y = DungeonHeight;

	tempUv[2].x = 0;
	tempUv[2].y = 0;

	tempUv[3].x = DungeonWidth;
	tempUv[3].y = 0;

	tempIndices[0] = 0 + positionVector.size();
	tempIndices[1] = 2 + positionVector.size();
	tempIndices[2] = 1 + positionVector.size();

	tempIndices[3] = 1 + positionVector.size();
	tempIndices[4] = 2 + positionVector.size();
	tempIndices[5] = 3 + positionVector.size();


	positionVector.insert(positionVector.end(), tempPos.begin(), tempPos.end());
	uvVector.insert(uvVector.end(), tempUv.begin(), tempUv.end());
	indicesVector.insert(indicesVector.end(), tempIndices.begin(), tempIndices.end());

}

void Dungeon::CreateWallsUTD(vector<XMFLOAT3> &positions, vector<XMFLOAT2> &uv, std::vector<unsigned int> &indices, vector<int> startPos, vector<int> lengths, int offset)
{
	for (int j = 0; j < lengths.size(); j++)
	{
		for (int i = 0; i < 4; i++) // front face
		{
			if (i == 0 || i == 2)
			{
				positions[j * 16 + i].x = startPos[j * 2] - 0.5;
				positions[j * 16 + i].y = i  * 2;
				positions[j * 16 + i].z = startPos[j * 2 + 1] - 0.5;
			}
			else if (i == 1 || i == 3)
			{
				positions[j * 16 + i].x = startPos[j * 2] - 0.5;
				positions[j * 16 + i].y = (i - 1) * 2;
				positions[j * 16 + i].z = startPos[j * 2 + 1] + lengths[j] + 0.5;
			}
		}

		uv[j * 16].x = lengths[j];
		uv[j * 16].y = 2 * 2;

		uv[j * 16 + 1].x = 0;
		uv[j * 16 + 1].y = 2 * 2;

		uv[j * 16 + 2].x = lengths[j];
		uv[j * 16 + 2].y = 0;

		uv[j * 16 + 3].x = 0;
		uv[j * 16 + 3].y = 0;


		indices[j * 24 + 0] = j * 16 + 0 + offset;
		indices[j * 24 + 1] = j * 16 + 2 + offset;
		indices[j * 24 + 2] = j * 16 + 1 + offset;

		indices[j * 24 + 3] = j * 16 + 1 + offset;
		indices[j * 24 + 4] = j * 16 + 2 + offset;
		indices[j * 24 + 5] = j * 16 + 3 + offset;


		for (int i = 0; i < 4; i++) // back face
		{
			if (i == 0 || i == 2)
			{
				positions[j * 16 + i + 4].x = startPos[j * 2] + 0.5;
				positions[j * 16 + i + 4].y = i * 2;
				positions[j * 16 + i + 4].z = startPos[j * 2 + 1] - 0.5;
			}
			else if (i == 1 || i == 3)
			{
				positions[j * 16 + i + 4].x = startPos[j * 2] + 0.5;
				positions[j * 16 + i + 4].y = (i - 1) * 2;
				positions[j * 16 + i + 4].z = startPos[j * 2 + 1] + lengths[j] + 0.5;
			}
		}


		uv[j * 16 + 4].x = 0;
		uv[j * 16 + 4].y = 2 * 2;

		uv[j * 16 + 5].x = lengths[j];
		uv[j * 16 + 5].y = 2 * 2;

		uv[j * 16 + 6].x = 0;
		uv[j * 16 + 6].y = 0;

		uv[j * 16 + 7].x = lengths[j];
		uv[j * 16 + 7].y = 0;


		indices[j * 24 + 6] = j * 16 + 1 + 4 + offset;
		indices[j * 24 + 7] = j * 16 + 2 + 4 + offset;
		indices[j * 24 + 8] = j * 16 + 0 + 4 + offset;

		indices[j * 24 + 9] = j * 16 + 3 + 4 + offset;
		indices[j * 24 + 10] = j * 16 + 2 + 4 + offset;
		indices[j * 24 + 11] = j * 16 + 1 + 4 + offset;


		for (int i = 0; i < 4; i++) // left face
		{
			if (i == 0 || i == 2)
			{
				positions[j * 16 + i + 8].x = startPos[j * 2] + 0.5;
				positions[j * 16 + i + 8].y = i * 2;
				positions[j * 16 + i + 8].z = startPos[j * 2 + 1] - 0.5;
			}
			else if (i == 1 || i == 3)
			{
				positions[j * 16 + i + 8].x = startPos[j * 2] - 0.5;
				positions[j * 16 + i + 8].y = (i - 1) * 2;
				positions[j * 16 + i + 8].z = startPos[j * 2 + 1] - 0.5;
			}
		}


		uv[j * 16 + 8].x = 1;
		uv[j * 16 + 8].y = 2 * 2;

		uv[j * 16 + 9].x = 0;
		uv[j * 16 + 9].y = 2 * 2;

		uv[j * 16 + 10].x = 1;
		uv[j * 16 + 10].y = 0;

		uv[j * 16 + 11].x = 0;
		uv[j * 16 + 11].y = 0;


		indices[j * 24 + 12] = j * 16 + 0 + 8 + offset;
		indices[j * 24 + 13] = j * 16 + 2 + 8 + offset;
		indices[j * 24 + 14] = j * 16 + 1 + 8 + offset;

		indices[j * 24 + 15] = j * 16 + 1 + 8 + offset;
		indices[j * 24 + 16] = j * 16 + 2 + 8 + offset;
		indices[j * 24 + 17] = j * 16 + 3 + 8 + offset;


		for (int i = 0; i < 4; i++) // right face
		{
			if (i == 0 || i == 2)
			{
				positions[j * 16 + i + 12].x = startPos[j * 2] - 0.5;
				positions[j * 16 + i + 12].y = i * 2;
				positions[j * 16 + i + 12].z = startPos[j * 2 + 1] + lengths[j] + 0.5;
			}
			else if (i == 1 || i == 3)
			{
				positions[j * 16 + i + 12].x = startPos[j * 2] + 0.5;
				positions[j * 16 + i + 12].y = (i - 1) * 2;
				positions[j * 16 + i + 12].z = startPos[j * 2 + 1] + lengths[j] + 0.5;
			}
		}


		uv[j * 16 + 12].x = 1;
		uv[j * 16 + 12].y = 2 * 2;

		uv[j * 16 + 13].x = 0;
		uv[j * 16 + 13].y = 2 * 2;

		uv[j * 16 + 14].x = 1;
		uv[j * 16 + 14].y = 0;

		uv[j * 16 + 15].x = 0;
		uv[j * 16 + 15].y = 0;


		indices[j * 24 + 18] = j * 16 + 0 + 12 + offset;
		indices[j * 24 + 19] = j * 16 + 2 + 12 + offset;
		indices[j * 24 + 20] = j * 16 + 1 + 12 + offset;

		indices[j * 24 + 21] = j * 16 + 1 + 12 + offset;
		indices[j * 24 + 22] = j * 16 + 2 + 12 + offset;
		indices[j * 24 + 23] = j * 16 + 3 + 12 + offset;

	}
}

void Dungeon::CreateWallsLTR(vector<XMFLOAT3> &positions, vector<XMFLOAT2> &uv, vector<unsigned int> &indices, vector<int> startPos, vector<int> lengths, int offset)
{
	for (int j = 0; j < lengths.size(); j++)
	{
		for (int i = 0; i < 4; i++) // front face
		{
			if (i == 0 || i == 2)
			{
				positions[j * 16 + i].x = startPos[j * 2] - 0.5;
				positions[j * 16 + i].y = i * 2;
				positions[j * 16 + i].z = startPos[j * 2 + 1] + 0.5;
			}
			else if (i == 1 || i == 3)
			{
				positions[j * 16 + i].x = startPos[j * 2] + lengths[j] + 0.5;
				positions[j * 16 + i].y = (i - 1) * 2;
				positions[j * 16 + i].z = startPos[j * 2 + 1] + 0.5;
			}
		}

		uv[j * 16].x = lengths[j];
		uv[j * 16].y = 2 * 2;

		uv[j * 16 + 1].x = 0;
		uv[j * 16 + 1].y = 2 * 2;

		uv[j * 16 + 2].x = lengths[j];
		uv[j * 16 + 2].y = 0;

		uv[j * 16 + 3].x = 0;
		uv[j * 16 + 3].y = 0;


		indices[j * 24 + 0] = j * 16 + 0 + offset;
		indices[j * 24 + 1] = j * 16 + 2 + offset;
		indices[j * 24 + 2] = j * 16 + 1 + offset;

		indices[j * 24 + 3] = j * 16 + 1 + offset;
		indices[j * 24 + 4] = j * 16 + 2 + offset;
		indices[j * 24 + 5] = j * 16 + 3 + offset;


		for (int i = 0; i < 4; i++) // back face
		{
			if (i == 0 || i == 2)
			{
				positions[j * 16 + i + 4].x = startPos[j * 2] - 0.5;
				positions[j * 16 + i + 4].y = i * 2;
				positions[j * 16 + i + 4].z = startPos[j * 2 + 1] - 0.5;
			}
			else if (i == 1 || i == 3)
			{
				positions[j * 16 + i + 4].x = startPos[j * 2] + lengths[j] + 0.5;
				positions[j * 16 + i + 4].y = (i - 1) * 2;
				positions[j * 16 + i + 4].z = startPos[j * 2 + 1] - 0.5;
			}
		}


		uv[j * 16 + 4].x = 0;
		uv[j * 16 + 4].y = 2 * 2;

		uv[j * 16 + 5].x = lengths[j];
		uv[j * 16 + 5].y = 2 * 2;

		uv[j * 16 + 6].x = 0;
		uv[j * 16 + 6].y = 0;

		uv[j * 16 + 7].x = lengths[j];
		uv[j * 16 + 7].y = 0;


		indices[j * 24 + 6] = j * 16 + 1 + 4 + offset;
		indices[j * 24 + 7] = j * 16 + 2 + 4 + offset;
		indices[j * 24 + 8] = j * 16 + 0 + 4 + offset;

		indices[j * 24 + 9] = j * 16 + 3 + 4 + offset;
		indices[j * 24 + 10] = j * 16 + 2 + 4 + offset;
		indices[j * 24 + 11] = j * 16 + 1 + 4 + offset;


		for (int i = 0; i < 4; i++) // left face
		{
			if (i == 0 || i == 2)
			{
				positions[j * 16 + i + 8].x = startPos[j * 2] - 0.5;
				positions[j * 16 + i + 8].y = i * 2;
				positions[j * 16 + i + 8].z = startPos[j * 2 + 1] - 0.5;
			}
			else if (i == 1 || i == 3)
			{
				positions[j * 16 + i + 8].x = startPos[j * 2] - 0.5;
				positions[j * 16 + i + 8].y = (i - 1) * 2;
				positions[j * 16 + i + 8].z = startPos[j * 2 + 1] + 0.5;
			}
		}


		uv[j * 16 + 8].x = 1;
		uv[j * 16 + 8].y = 2 * 2;

		uv[j * 16 + 9].x = 0;
		uv[j * 16 + 9].y = 2 * 2;

		uv[j * 16 + 10].x = 1;
		uv[j * 16 + 10].y = 0;

		uv[j * 16 + 11].x = 0;
		uv[j * 16 + 11].y = 0;


		indices[j * 24 + 12] = j * 16 + 0 + 8 + offset;
		indices[j * 24 + 13] = j * 16 + 2 + 8 + offset;
		indices[j * 24 + 14] = j * 16 + 1 + 8 + offset;

		indices[j * 24 + 15] = j * 16 + 1 + 8 + offset;
		indices[j * 24 + 16] = j * 16 + 2 + 8 + offset;
		indices[j * 24 + 17] = j * 16 + 3 + 8 + offset;


		for (int i = 0; i < 4; i++) // right face
		{
			if (i == 0 || i == 2)
			{
				positions[j * 16 + i + 12].x = startPos[j * 2] + lengths[j] + 0.5;
				positions[j * 16 + i + 12].y = i * 2;
				positions[j * 16 + i + 12].z = startPos[j * 2 + 1] + 0.5;
			}
			else if (i == 1 || i == 3)
			{
				positions[j * 16 + i + 12].x = startPos[j * 2] + lengths[j] + 0.5;
				positions[j * 16 + i + 12].y = (i - 1) * 2;
				positions[j * 16 + i + 12].z = startPos[j * 2 + 1] - 0.5;
			}
		}


		uv[j * 16 + 12].x = 1;
		uv[j * 16 + 12].y = 2 * 2;

		uv[j * 16 + 13].x = 0;
		uv[j * 16 + 13].y = 2 * 2;

		uv[j * 16 + 14].x = 1;
		uv[j * 16 + 14].y = 0;

		uv[j * 16 + 15].x = 0;
		uv[j * 16 + 15].y = 0;


		indices[j * 24 + 18] = j * 16 + 0 + 12 + offset;
		indices[j * 24 + 19] = j * 16 + 2 + 12 + offset;
		indices[j * 24 + 20] = j * 16 + 1 + 12 + offset;

		indices[j * 24 + 21] = j * 16 + 1 + 12 + offset;
		indices[j * 24 + 22] = j * 16 + 2 + 12 + offset;
		indices[j * 24 + 23] = j * 16 + 3 + 12 + offset;

	}

}

std::vector<SubMeshInfo> Dungeon::GetSubMeshInfo()
{
	return wallInfo;
}

std::vector<DirectX::XMFLOAT3>& Dungeon::GetPosVector()
{
	return positionVector;
}

std::vector<DirectX::XMFLOAT2>& Dungeon::GetUvVector()
{
	return uvVector;
}

std::vector<unsigned int>& Dungeon::GetIndicesVector()
{
	return indicesVector;
}

int Dungeon::getTile(int widthPos, int heightPos)
{
	return tiles[widthPos][heightPos];
}
