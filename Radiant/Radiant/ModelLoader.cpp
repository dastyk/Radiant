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
		throw(ErrorMsg(6000003U, L"Unable to open model file.\n"));
	}

	uint32_t vertexCount = 0;
	uint32_t indexCount = 0;
	uint32_t submeshCount = 0;

	fin.read((char*)&vertexCount, sizeof(uint32_t));
	fin.read((char*)&indexCount, sizeof(uint32_t));

	VertexLayout* vertices = nullptr;
	uint32_t* indices = nullptr;

	try {
		vertices = new VertexLayout[vertexCount];
		indices = new uint32_t[indexCount];
	}
	catch (std::exception& e)
	{
		e;
		SAFE_DELETE_ARRAY(vertices);
		SAFE_DELETE_ARRAY(indices);
		throw(ErrorMsg(6000004U, L"Failed to allocate memory in model loader.\n"));
	}

	fin.read((char*)vertices, sizeof(VertexLayout) * vertexCount);
	fin.read((char*)indices, sizeof(uint32_t) * indexCount);

	fin.read((char*)&submeshCount, sizeof(uint32_t));

	SubMeshInfo* submeshes = new SubMeshInfo[submeshCount];
	fin.read((char*)submeshes, sizeof(SubMeshInfo) * submeshCount);

	fin.close();

	DirectX::XMFLOAT3* position = nullptr;
	DirectX::XMFLOAT3* normal = nullptr;
	DirectX::XMFLOAT3* tangent = nullptr;
	DirectX::XMFLOAT3* binormal = nullptr;
	DirectX::XMFLOAT2* texcoords = nullptr;

	try {
		position = new DirectX::XMFLOAT3[vertexCount];
		normal = new DirectX::XMFLOAT3[vertexCount];
		tangent = new DirectX::XMFLOAT3[vertexCount];
		binormal = new DirectX::XMFLOAT3[vertexCount];
		texcoords = new DirectX::XMFLOAT2[vertexCount];
	}
	catch (std::exception& e)
	{
		e;
		SAFE_DELETE_ARRAY(position);
		SAFE_DELETE_ARRAY(normal);
		SAFE_DELETE_ARRAY(tangent);
		SAFE_DELETE_ARRAY(binormal);
		SAFE_DELETE_ARRAY(texcoords);
		throw(ErrorMsg(6000004U, L"Failed to allocate memory in model loader.\n"));
	}

	for (uint32_t i = 0; i < vertexCount; ++i)
	{
		position[i] = vertices[i]._position;
		normal[i] = vertices[i]._normal;
		tangent[i] = vertices[i]._tangent;
		binormal[i] = vertices[i]._binormal;
		texcoords[i] = vertices[i]._texCoords;
	}

	Mesh* mesh = nullptr;

	try {
		mesh = new Mesh();
	}
	catch (std::exception& e)
	{
		e;
		throw(ErrorMsg(6000004U, L"Failed to allocate memory in model loader.\n"));
	}

	mesh->AddAttributeStream(Mesh::AttributeType::Position, vertexCount, (float*)position, indexCount, indices);
	mesh->AddAttributeStream(Mesh::AttributeType::Normal, vertexCount, (float*)normal, indexCount, indices);
	mesh->AddAttributeStream(Mesh::AttributeType::Tangent, vertexCount, (float*)tangent, indexCount, indices);
	mesh->AddAttributeStream(Mesh::AttributeType::Binormal, vertexCount, (float*)binormal, indexCount, indices);
	mesh->AddAttributeStream(Mesh::AttributeType::TexCoord, vertexCount, (float*)texcoords, indexCount, indices);

	for (uint32_t i = 0; i < submeshCount; ++i)
	{
		mesh->AddBatch(submeshes[i].indexStart, submeshes[i].count);
	}
	
	delete[] vertices;
	delete[] indices;
	delete[] position;
	delete[] normal;
	delete[] tangent;
	delete[] binormal;
	delete[] texcoords;

	return mesh;
}