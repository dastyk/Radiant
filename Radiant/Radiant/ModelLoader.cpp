#include "ModelLoader.h"
#include "General.h"
#include <fstream>
#include <string>
#include <vector>

ModelLoader::ModelLoader()
{
	//Empty
}

ModelLoader::~ModelLoader()
{

}

Mesh* ModelLoader::LoadModel(std::string filename)
{
	std::ifstream fin;
	fin.open(filename, std::ios_base::in | std::ios_base::binary);
	if(!fin.is_open())
	{
		throw(ErrorMsg(6000004U, L"Unable to open model file.\n"));
	}

	uint32_t vertexCount;
	uint32_t indexCount;
	uint32_t submeshCount;

	fin.read((char*)&vertexCount, sizeof(uint32_t));
	fin.read((char*)&indexCount, sizeof(uint32_t));

	VertexLayout* vertices = new VertexLayout[vertexCount];
	uint32_t* indices = new uint32_t[indexCount];

	fin.read((char*)vertices, sizeof(VertexLayout) * vertexCount);
	fin.read((char*)indices, sizeof(uint32_t) * indexCount);

	fin.read((char*)submeshCount, sizeof(uint32_t));

	SubMeshInfo* submeshes = new SubMeshInfo[submeshCount];
	fin.read((char*)submeshes, sizeof(SubMeshInfo) * submeshCount);

	fin.close();

	DirectX::XMFLOAT3* position = new DirectX::XMFLOAT3[vertexCount];
	DirectX::XMFLOAT3* normal = new DirectX::XMFLOAT3[vertexCount];
	DirectX::XMFLOAT3* tangent = new DirectX::XMFLOAT3[vertexCount];
	DirectX::XMFLOAT3* binormal = new DirectX::XMFLOAT3[vertexCount];
	DirectX::XMFLOAT2* texcoords = new DirectX::XMFLOAT2[vertexCount];

	for (int i = 0; i < vertexCount; ++i)
	{
		position[i] = vertices[i]._position;
		normal[i] = vertices[i]._normal;
		tangent[i] = vertices[i]._tangent;
		binormal[i] = vertices[i]._binormal;
		texcoords[i] = vertices[i]._texCoords;
	}

	Mesh* mesh = new Mesh();

	mesh->AddAttributeStream(Mesh::AttributeType::Position, vertexCount, (float*)position, indexCount, indices);
	mesh->AddAttributeStream(Mesh::AttributeType::Normal, vertexCount, (float*)normal, indexCount, indices);
	mesh->AddAttributeStream(Mesh::AttributeType::Tangent, vertexCount, (float*)tangent, indexCount, indices);
	mesh->AddAttributeStream(Mesh::AttributeType::Binormal, vertexCount, (float*)binormal, indexCount, indices);
	mesh->AddAttributeStream(Mesh::AttributeType::TexCoord, vertexCount, (float*)texcoords, indexCount, indices);

	for (int i = 0; i < submeshCount; ++i)
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