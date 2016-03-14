#include "Dungeon.h"
#include "System.h"
using namespace DirectX;
using namespace std;

Dungeon::Dungeon(int width, int height, EntityBuilder* builder) : _builder(builder)
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

	generateDungeon(0);
}

Dungeon::Dungeon(int side, int minimumExtent, int maximumExtent, float percentToCover, EntityBuilder* builder, unsigned int level) : _builder(builder)
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

	generateDungeon(level);
}

Dungeon::~Dungeon()
{
	for (int i = 0; i < DungeonWidth; i++)
	{
		delete[] tiles[i];
	}

	for (int i = 0; i < walls.size(); i++)
	{
		_builder->GetEntityController()->ReleaseEntity(walls[i]);
	}
	for (int i = 0; i < floorroof.size(); i++)
	{
		_builder->GetEntityController()->ReleaseEntity(floorroof[i]);
	}
	walls.clear();
	floorroof.clear();
	delete[] tiles;

	delete rooms;
}

void Dungeon::generateDungeon(unsigned int level)
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
						generateDungeonRecursive();
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

			percentCovered = 1 - (float)occupiedTiles / ((float)DungeonWidth * (float)DungeonHeight);

		}
	}

	generateCorridors();
	removeWalls();
	GenerateGraphicalData(level);

}

void Dungeon::generateDungeonRecursive()
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
						generateDungeonRecursive();
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

			percentCovered = 1 - (float)occupiedTiles / ((float)DungeonWidth * (float)DungeonHeight);

		}
	}

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

void Dungeon::GenerateGraphicalData(unsigned int level)
{
	for (int i = 0; i < DungeonWidth; i++)
	{
		for (int j = 0; j < DungeonHeight; j++)
		{
			if (tiles[i][j] == 0)
			{
				freePositions.push_back(FreePositions(i, j));
			}
		}
	}
	if (freePositions.size() == 0)
	{
		generateDungeon(level);
		return;
	}


	std::string name = "Assets/Textures/Wall_" + to_string(level%1) + "_";

	for (int i = 0; i < DungeonWidth; i++)
	{
		for (int j = 0; j < DungeonHeight; j++)
		{
			if (tiles[i][j] == 1)
			{
				Entity ent = _builder->CreateObjectWithEmissive(
					XMVectorSet((float)i, 1.5f, (float)j, 0.0f),
					XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
					XMVectorSet(1.0f, 3.0f, 1.0f, 0.0f),
					"Assets/Models/cube.arf",
					name + "Dif.png",
					name + "NM.png",
					name + "Disp.png",
					name + "Roughness.png",
					name + "Glow.png");
				_builder->Bounding()->CreateBoundingBox(ent, 0.5f, 0.5f, 0.5f);
				_builder->Material()->SetMaterialProperty(ent, 0, "Metallic", 0.0f, "Shaders/GBufferEmissive.hlsl");
				_builder->Material()->SetMaterialProperty(ent, "TexCoordScaleV", 3.0f, "Shaders/GBufferEmissive.hlsl");
				_builder->Material()->SetMaterialProperty(ent, "ParallaxBias", -0.01f, "Shaders/GBufferEmissive.hlsl");
				_builder->Material()->SetMaterialProperty(ent, "ParallaxScaling", 0.02f, "Shaders/GBufferEmissive.hlsl");
				_builder->Material()->SetMaterialProperty(ent, "EmissiveIntensity", 1.0f, "Shaders/GBufferEmissive.hlsl");
				_builder->Material()->SetMaterialProperty(ent, "BlurIntensity", 1.0f, "Shaders/GBufferEmissive.hlsl");
				_builder->Material()->SetMaterialProperty(ent, "EmissiveColor", XMFLOAT3(0.0f,0.0f,1.0f), "Shaders/GBufferEmissive.hlsl");
				_builder->Transform()->MoveForward(ent, 0.0f);

				walls.push_back(ent);

			}
		}
	}

	std::vector<Entity> newV = walls;
	Entity ent = _builder->CreateObject(
		XMVectorSet(DungeonWidth/2.0f, -0.5, DungeonHeight/2.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet((float)(DungeonWidth), 1.0f, (float)(DungeonHeight), 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/Floor_0_Dif.png",
		"Assets/Textures/Floor_0_NM.png",
		"Assets/Textures/Floor_0_Disp.png",
		"Assets/Textures/Floor_0_Roughness.png");
	_builder->Bounding()->CreateBoundingBox(ent, 0.5f, 0.5f, 0.5f);
	_builder->Material()->SetMaterialProperty(ent, 0, "Metallic", 0.0f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(ent, "TexCoordScaleU", (float)DungeonWidth, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(ent, "TexCoordScaleV",(float)DungeonHeight, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(ent, "ParallaxBias", -0.05f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(ent, "ParallaxScaling", 0.12f, "Shaders/GBuffer.hlsl");
	_builder->Transform()->MoveForward(ent, 0.0f);
	floorroof.push_back(ent);


	ent = _builder->CreateObject(
		XMVectorSet(DungeonWidth / 2.0f, 2.5, DungeonHeight / 2.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet((float)DungeonWidth, 1.0f, (float)DungeonHeight, 0.0f),
		"Assets/Models/cube.arf",
		"Assets/Textures/Floor_0_Dif.png",
		"Assets/Textures/Floor_0_NM.png",
		"Assets/Textures/Floor_0_Disp.png",
		"Assets/Textures/Floor_0_Roughness.png");
	_builder->Bounding()->CreateBoundingBox(ent, 0.5f, 0.5f, 0.5f);
	//_builder->Material()->SetMaterialProperty(ent, 0, "Roughness", 0.99f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(ent, 0, "Metallic", 0.1f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(ent, "TexCoordScaleU", (float)DungeonWidth, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(ent, "TexCoordScaleV", (float)DungeonHeight, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(ent, "ParallaxBias", -0.02f, "Shaders/GBuffer.hlsl");
	_builder->Material()->SetMaterialProperty(ent, "ParallaxScaling", 0.04f, "Shaders/GBuffer.hlsl");
	_builder->Transform()->MoveForward(ent, 0.0f);
	floorroof.push_back(ent);





}

void Dungeon::CreateWallsUTD(vector<XMFLOAT3> &positions, vector<XMFLOAT2> &uv, std::vector<unsigned int> &indices, vector<int> startPos, vector<int> lengths, int offset)
{
	for (int j = 0; j < lengths.size(); j++)
	{
		for (int counter = 0; counter < lengths[j]; counter++)
		{
			tiles[startPos[j * 2]][startPos[j * 2 + 1] + counter] = 1;
		}

		walls.push_back(_builder->EntityC().Create());
		_builder->Transform()->CreateTransform(walls[walls.size() - 1]);

		_builder->Mesh()->CreateStaticMesh(walls[walls.size() - 1], "Assets/Models/cube.arf");
		_builder->Material()->BindMaterial(walls[walls.size() - 1], "Shaders/GBuffer.hlsl");
		_builder->Material()->SetEntityTexture(walls[walls.size() - 1], "DiffuseMap", L"Assets/Textures/ft_stone01_c.png");
		_builder->Material()->SetEntityTexture(walls[walls.size() - 1], "NormalMap", L"Assets/Textures/ft_stone01_n.png");
		_builder->Material()->SetMaterialProperty(walls[walls.size() - 1], 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
		_builder->Material()->SetMaterialProperty(walls[walls.size() - 1], 0, "Metallic", 0.1f, "Shaders/GBuffer.hlsl");

		_builder->GetEntityController()->Transform()->SetScale(walls[walls.size() - 1], XMFLOAT3(1.0f, 2.0f, lengths[j] + 1.0f));
		_builder->GetEntityController()->Transform()->SetPosition(walls[walls.size() - 1], XMFLOAT3((float)startPos[j * 2], 0.5f, startPos[j * 2 + 1] + lengths[j] / 2.0f));

	}
}

void Dungeon::CreateWallsLTR(vector<XMFLOAT3> &positions, vector<XMFLOAT2> &uv, vector<unsigned int> &indices, vector<int> startPos, vector<int> lengths, int offset)
{
	for (int j = 0; j < lengths.size(); j++)
	{

		for (int counter = 0; counter < lengths[j]; counter++)
		{
			tiles[startPos[j * 2] + counter ][startPos[j * 2 + 1]] = 1;
		}

		walls.push_back(_builder->EntityC().Create());
		_builder->Transform()->CreateTransform(walls[walls.size() - 1]);

		_builder->Mesh()->CreateStaticMesh(walls[walls.size() - 1], "Assets/Models/cube.arf");
		_builder->Material()->BindMaterial(walls[walls.size() - 1], "Shaders/GBuffer.hlsl");
		_builder->Material()->SetEntityTexture(walls[walls.size() - 1], "DiffuseMap", L"Assets/Textures/ft_stone01_c.png");
		_builder->Material()->SetEntityTexture(walls[walls.size() - 1], "NormalMap", L"Assets/Textures/ft_stone01_n.png");
		_builder->Material()->SetMaterialProperty(walls[walls.size() - 1], 0, "Roughness", 1.0f, "Shaders/GBuffer.hlsl");
		_builder->Material()->SetMaterialProperty(walls[walls.size() - 1], 0, "Metallic", 0.1f, "Shaders/GBuffer.hlsl");

		_builder->GetEntityController()->Transform()->SetScale(walls[walls.size() - 1], XMFLOAT3(lengths[j] + 1.0f, 2.0f, 1.0f));
		_builder->GetEntityController()->Transform()->SetPosition(walls[walls.size() - 1], XMFLOAT3((float)(startPos[j * 2] + lengths[j] / 2.0f), 0.5f, (float)(startPos[j * 2 + 1])));
		// CHANGE UV SCALING LATER WHEN AVAILABLE!!!

		//for (int i = 0; i < 4; i++) // front face
		//{
		//	if (i == 0 || i == 2)
		//	{
		//		positions[j * 16 + i].x = startPos[j * 2] - 0.5;
		//		positions[j * 16 + i].y = i * 2;
		//		positions[j * 16 + i].z = startPos[j * 2 + 1] + 0.5;
		//	}
		//	else if (i == 1 || i == 3)
		//	{
		//		positions[j * 16 + i].x = startPos[j * 2] + lengths[j] + 0.5;
		//		positions[j * 16 + i].y = (i - 1) * 2;
		//		positions[j * 16 + i].z = startPos[j * 2 + 1] + 0.5;
		//	}
		//}

		//uv[j * 16].x = lengths[j];
		//uv[j * 16].y = 2 * 2;

		//uv[j * 16 + 1].x = 0;
		//uv[j * 16 + 1].y = 2 * 2;

		//uv[j * 16 + 2].x = lengths[j];
		//uv[j * 16 + 2].y = 0;

		//uv[j * 16 + 3].x = 0;
		//uv[j * 16 + 3].y = 0;


		//indices[j * 24 + 0] = j * 16 + 0 + offset;
		//indices[j * 24 + 1] = j * 16 + 2 + offset;
		//indices[j * 24 + 2] = j * 16 + 1 + offset;

		//indices[j * 24 + 3] = j * 16 + 1 + offset;
		//indices[j * 24 + 4] = j * 16 + 2 + offset;
		//indices[j * 24 + 5] = j * 16 + 3 + offset;


		//for (int i = 0; i < 4; i++) // back face
		//{
		//	if (i == 0 || i == 2)
		//	{
		//		positions[j * 16 + i + 4].x = startPos[j * 2] - 0.5;
		//		positions[j * 16 + i + 4].y = i * 2;
		//		positions[j * 16 + i + 4].z = startPos[j * 2 + 1] - 0.5;
		//	}
		//	else if (i == 1 || i == 3)
		//	{
		//		positions[j * 16 + i + 4].x = startPos[j * 2] + lengths[j] + 0.5;
		//		positions[j * 16 + i + 4].y = (i - 1) * 2;
		//		positions[j * 16 + i + 4].z = startPos[j * 2 + 1] - 0.5;
		//	}
		//}


		//uv[j * 16 + 4].x = 0;
		//uv[j * 16 + 4].y = 2 * 2;

		//uv[j * 16 + 5].x = lengths[j];
		//uv[j * 16 + 5].y = 2 * 2;

		//uv[j * 16 + 6].x = 0;
		//uv[j * 16 + 6].y = 0;

		//uv[j * 16 + 7].x = lengths[j];
		//uv[j * 16 + 7].y = 0;


		//indices[j * 24 + 6] = j * 16 + 1 + 4 + offset;
		//indices[j * 24 + 7] = j * 16 + 2 + 4 + offset;
		//indices[j * 24 + 8] = j * 16 + 0 + 4 + offset;

		//indices[j * 24 + 9] = j * 16 + 3 + 4 + offset;
		//indices[j * 24 + 10] = j * 16 + 2 + 4 + offset;
		//indices[j * 24 + 11] = j * 16 + 1 + 4 + offset;


		//for (int i = 0; i < 4; i++) // left face
		//{
		//	if (i == 0 || i == 2)
		//	{
		//		positions[j * 16 + i + 8].x = startPos[j * 2] - 0.5;
		//		positions[j * 16 + i + 8].y = i * 2;
		//		positions[j * 16 + i + 8].z = startPos[j * 2 + 1] - 0.5;
		//	}
		//	else if (i == 1 || i == 3)
		//	{
		//		positions[j * 16 + i + 8].x = startPos[j * 2] - 0.5;
		//		positions[j * 16 + i + 8].y = (i - 1) * 2;
		//		positions[j * 16 + i + 8].z = startPos[j * 2 + 1] + 0.5;
		//	}
		//}


		//uv[j * 16 + 8].x = 1;
		//uv[j * 16 + 8].y = 2 * 2;

		//uv[j * 16 + 9].x = 0;
		//uv[j * 16 + 9].y = 2 * 2;

		//uv[j * 16 + 10].x = 1;
		//uv[j * 16 + 10].y = 0;

		//uv[j * 16 + 11].x = 0;
		//uv[j * 16 + 11].y = 0;


		//indices[j * 24 + 12] = j * 16 + 0 + 8 + offset;
		//indices[j * 24 + 13] = j * 16 + 2 + 8 + offset;
		//indices[j * 24 + 14] = j * 16 + 1 + 8 + offset;

		//indices[j * 24 + 15] = j * 16 + 1 + 8 + offset;
		//indices[j * 24 + 16] = j * 16 + 2 + 8 + offset;
		//indices[j * 24 + 17] = j * 16 + 3 + 8 + offset;


		//for (int i = 0; i < 4; i++) // right face
		//{
		//	if (i == 0 || i == 2)
		//	{
		//		positions[j * 16 + i + 12].x = startPos[j * 2] + lengths[j] + 0.5;
		//		positions[j * 16 + i + 12].y = i * 2;
		//		positions[j * 16 + i + 12].z = startPos[j * 2 + 1] + 0.5;
		//	}
		//	else if (i == 1 || i == 3)
		//	{
		//		positions[j * 16 + i + 12].x = startPos[j * 2] + lengths[j] + 0.5;
		//		positions[j * 16 + i + 12].y = (i - 1) * 2;
		//		positions[j * 16 + i + 12].z = startPos[j * 2 + 1] - 0.5;
		//	}
		//}


		//uv[j * 16 + 12].x = 1;
		//uv[j * 16 + 12].y = 2 * 2;

		//uv[j * 16 + 13].x = 0;
		//uv[j * 16 + 13].y = 2 * 2;

		//uv[j * 16 + 14].x = 1;
		//uv[j * 16 + 14].y = 0;

		//uv[j * 16 + 15].x = 0;
		//uv[j * 16 + 15].y = 0;


		//indices[j * 24 + 18] = j * 16 + 0 + 12 + offset;
		//indices[j * 24 + 19] = j * 16 + 2 + 12 + offset;
		//indices[j * 24 + 20] = j * 16 + 1 + 12 + offset;

		//indices[j * 24 + 21] = j * 16 + 1 + 12 + offset;
		//indices[j * 24 + 22] = j * 16 + 2 + 12 + offset;
		//indices[j * 24 + 23] = j * 16 + 3 + 12 + offset;

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

const std::vector<FreePositions>& Dungeon::GetFreePositions()
{
	return freePositions;
}

const std::vector<Entity>& Dungeon::GetWalls() const
{
	return walls;
}

const std::vector<Entity>& Dungeon::GetFloorRoof() const
{
	return floorroof;
}

const FreePositions Dungeon::GetunoccupiedSpace()
{
	int randomnr;

	bool done = false;
	if (occupied.size() == freePositions.size())
		return freePositions[0];

	while (!done)
	{
		done = true;
		randomnr = (int)((rand() % (freePositions.size() * 100)) / 100);
		for (auto& als : occupied)
		{
			if (als == randomnr)
			{
				done = false;
			}
		}
		if (done)
		{
			occupied.push_back(randomnr);
			return freePositions[randomnr];
		}
	}
	return FreePositions(0,0);
}


