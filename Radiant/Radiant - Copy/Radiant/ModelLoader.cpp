#include "ModelLoader.h"
#include "General.h"
#include "Utils.h"
#include <fstream>
#include <string>
#include <vector>

ModelLoader::ModelLoader()
{
	//Empty
}

ModelLoader::~ModelLoader()
{
	//empty
}

Mesh* ModelLoader::LoadModel(std::string filename)
{
	std::ifstream fin;
	fin.open(filename, std::ios_base::in | std::ios_base::binary);
	if(!fin.is_open())
	{
		throw(ErrorMsg(6000003U, L"Unable to open model file.\n", S2WS(filename)));
	}

	uint32_t posCount = 0;
	uint32_t posIndCount = 0;

	uint32_t ntbCount = 0;//normal tangent binormal
	uint32_t ntbIndCount = 0;

	uint32_t texCount = 0;
	uint32_t texIndCount = 0;

	uint32_t subMeshCount = 0;

	fin.read((char*)&posCount, sizeof(uint32_t));
	fin.read((char*)&posIndCount, sizeof(uint32_t));

	fin.read((char*)&ntbCount, sizeof(uint32_t));
	fin.read((char*)&ntbIndCount, sizeof(uint32_t));

	fin.read((char*)&texCount, sizeof(uint32_t));
	fin.read((char*)&texIndCount, sizeof(uint32_t));

	fin.read((char*)&subMeshCount, sizeof(uint32_t));

	DirectX::XMFLOAT3* positions = nullptr;
	uint32_t* positionIndices = nullptr;

	DirectX::XMFLOAT3* normals = nullptr;
	DirectX::XMFLOAT3* tangents = nullptr;
	DirectX::XMFLOAT3* binormals = nullptr;
	uint32_t* ntbIndices = nullptr;

	DirectX::XMFLOAT2* texCoords = nullptr;
	uint32_t* texCoordIndices = nullptr;

	SubMeshInfo* subMeshes = nullptr;

	try {
		positions = new DirectX::XMFLOAT3[posCount];
		positionIndices = new uint32_t[posIndCount];

		normals = new DirectX::XMFLOAT3[ntbCount];
		tangents = new DirectX::XMFLOAT3[ntbCount];
		binormals = new DirectX::XMFLOAT3[ntbCount];
		ntbIndices = new uint32_t[ntbIndCount];

		texCoords = new DirectX::XMFLOAT2[texCount];
		texCoordIndices = new uint32_t[texIndCount];

		subMeshes = new SubMeshInfo[subMeshCount];
	}
	catch (std::exception& e)
	{
		e;
		SAFE_DELETE_ARRAY(positions);
		SAFE_DELETE_ARRAY(positionIndices);
		SAFE_DELETE_ARRAY(normals);
		SAFE_DELETE_ARRAY(tangents);
		SAFE_DELETE_ARRAY(binormals);
		SAFE_DELETE_ARRAY(ntbIndices);
		SAFE_DELETE_ARRAY(texCoords);
		SAFE_DELETE_ARRAY(texCoordIndices);
		SAFE_DELETE_ARRAY(subMeshes);
		throw(ErrorMsg(6000004U, L"Failed to allocate memory in model loader.\n"));
	}

	fin.read((char*)positions, sizeof(DirectX::XMFLOAT3) * posCount);
	fin.read((char*)positionIndices, sizeof(uint32_t) * posIndCount);

	fin.read((char*)normals, sizeof(DirectX::XMFLOAT3) * ntbCount);
	fin.read((char*)tangents, sizeof(DirectX::XMFLOAT3) * ntbCount);
	fin.read((char*)binormals, sizeof(DirectX::XMFLOAT3) * ntbCount);
	fin.read((char*)ntbIndices, sizeof(uint32_t) * ntbIndCount);

	fin.read((char*)texCoords, sizeof(DirectX::XMFLOAT2) * texCount);
	fin.read((char*)texCoordIndices, sizeof(uint32_t) * texIndCount);

	fin.read((char*)subMeshes, sizeof(SubMeshInfo) * subMeshCount);

	fin.close();

	

	Mesh* mesh = nullptr;

	try {
		mesh = new Mesh();
	}
	catch (std::exception& e)
	{
		e;
		throw(ErrorMsg(6000004U, L"Failed to allocate memory in model loader.\n"));
	}

	mesh->AddAttributeStream(Mesh::AttributeType::Position, posCount, (float*)positions, posIndCount, positionIndices);
	mesh->AddAttributeStream(Mesh::AttributeType::Normal, ntbCount, (float*)normals, ntbIndCount, ntbIndices);
	mesh->AddAttributeStream(Mesh::AttributeType::Tangent, ntbCount, (float*)tangents, ntbIndCount, ntbIndices);
	mesh->AddAttributeStream(Mesh::AttributeType::Binormal, ntbCount, (float*)binormals, ntbIndCount, ntbIndices);
	mesh->AddAttributeStream(Mesh::AttributeType::TexCoord, texCount, (float*)texCoords, texIndCount, texCoordIndices);

	for (uint32_t i = 0; i < subMeshCount; ++i)
	{
		mesh->AddBatch(subMeshes[i].indexStart, subMeshes[i].count);
	}

	
	SAFE_DELETE_ARRAY(positions);
	SAFE_DELETE_ARRAY(positionIndices);
	SAFE_DELETE_ARRAY(normals);
	SAFE_DELETE_ARRAY(tangents);
	SAFE_DELETE_ARRAY(binormals);
	SAFE_DELETE_ARRAY(ntbIndices);
	SAFE_DELETE_ARRAY(texCoords);
	SAFE_DELETE_ARRAY(texCoordIndices);
	SAFE_DELETE_ARRAY(subMeshes);

	return mesh;
}