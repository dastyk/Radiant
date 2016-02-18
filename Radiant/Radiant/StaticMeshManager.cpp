#include "StaticMeshManager.h"

#include <algorithm>
#include "System.h"
#include "Utils.h"
#include "OBJLoader.h"

using namespace std;
using namespace DirectX;

StaticMeshManager::StaticMeshManager( TransformManager& transformManager, MaterialManager& materialManager ) : _graphics(*System::GetGraphics())
{
	_graphics.AddRenderProvider( this );
	
	transformManager.TransformChanged += Delegate<void( const Entity&, const XMMATRIX&, const XMVECTOR&, const XMVECTOR&, const XMVECTOR& )>::Make<StaticMeshManager, &StaticMeshManager::_TransformChanged>( this );
	//Material of certain submesh
	materialManager.SetMaterialChangeCallback([this](Entity entity,const ShaderData* material, uint32_t subMesh)
	{
		_MaterialChanged(entity, material, subMesh);
	});

	//Material of entire mesh
	materialManager.SetMaterialEntireEntityCallback([this](Entity entity, const ShaderData* material)
	{
		MaterialChanged(entity, material);
	});

	materialManager.SetMaterialCreatedCallback([this](Entity entity,const ShaderData* material)
	{
		auto find = _entityToIndex.find(entity);
		if (find != _entityToIndex.end())
		{
			_SetDefaultMaterials(entity, material);
		}
	});


}

StaticMeshManager::~StaticMeshManager()
{
	std::vector<uint32_t> vbIndices;
	vbIndices.reserve(_loadedFiles.size());
	std::vector<uint32_t> ibIndices;
	ibIndices.reserve(_loadedFiles.size());
	for (auto file : _loadedFiles)
	{
		SAFE_DELETE(file.second.Mesh);
		vbIndices.push_back(file.second.VertexBuffer);
		ibIndices.push_back(file.second.IndexBuffer);
	}

	_graphics.ReleaseStaticMeshBuffers(vbIndices, ibIndices);

	_loadedFiles.clear();
}
//void StaticMeshManager::GatherJobs( std::function</*const Material**/void(RenderJob&)> ProvideJob )
//{
//	//RenderJob job;
//	//
//
//	//for ( auto& mesh : _meshes )
//	//{
//	//	job.VertexBuffer = mesh.VertexBuffer;
//	//	job.IndexBuffer = mesh.IndexBuffer;
//	//	job.Transform = mesh.Transform;
//
//	//	for ( auto& meshPart : mesh.Parts )
//	//	{
//	//		job.IndexStart = meshPart.IndexStart;
//	//		job.IndexCount = meshPart.IndexCount;
//	//		//job.Material = meshPart.Material;
//
//	//		// Provide job, and if no material was set, a default one is
//	//		// returned for us to use next time.
//	//		//if ( const Material *newMat = ProvideJob( job ) )
//	//		//	meshPart.Material = *newMat;
//	//		ProvideJob( job );
//	//	}
//	//}
//
//
//
//
//}
void StaticMeshManager::GatherJobs(RenderJobMap& jobs)
{
	for (auto& mesh : _meshes)
	{
		for (auto& meshPart : mesh.Parts)
		{
			if (meshPart.Visible)
			{
				RenderJobMap4& j = jobs[meshPart.Material->Shader][mesh.VertexBuffer][mesh.IndexBuffer];
				meshPart.translation = &mesh.Transform;
				j.push_back(&meshPart);
			}
		}
	}
}
void StaticMeshManager::CreateStaticMesh(Entity entity, const char *filename)
{
	LPCSTR st = filename;
	string fn = PathFindFileNameA(st);
	auto get = _loadedFiles.find(fn);
	if (get != _loadedFiles.end())
	{
		MeshData meshData = get->second;
		meshData.OwningEntity = entity;
		XMStoreFloat4x4(&meshData.Transform, XMMatrixIdentity());

		for_each( meshData.Parts.begin(), meshData.Parts.end(), []( MeshPart &part )
		{
			part.Visible = true;
		} );

		_entityToIndex[entity] = static_cast<int>(_meshes.size());
		_meshes.push_back(move(meshData));
		return;

	}

	Mesh* mesh;
	string s( filename );
	try
	{
		if ( s.find( ".obj", s.length() - 4 ) != string::npos )
		{
			OBJLoader loader;
			mesh = loader.Load( filename );
			mesh->CalcNTB();
		}
		else
		{
			mesh = System::GetFileHandler()->LoadModel( filename );
		}
	}
	catch (ErrorMsg& msg)
	{
		throw msg;
	}

	//TraceDebug( "T-junctions found in %s: %d", filename, mesh->FixTJunctions() );
	mesh->FlipPositionZ();
	mesh->FlipNormals();
	mesh->FlipTangents();
	mesh->FlipBitangents();
	mesh->InvertV();

	uint32_t vertexBufferIndex = 0;
	uint32_t indexBufferIndex = 0;
	if (!_graphics.CreateMeshBuffers(mesh, vertexBufferIndex, indexBufferIndex))
	{
		SAFE_DELETE(mesh);
		TraceDebug("Failed to create buffers for file: '%s'", filename);

		return;
	}

	MeshData meshData;
	meshData.OwningEntity = entity;
	XMStoreFloat4x4(&meshData.Transform, XMMatrixIdentity());
	meshData.VertexBuffer = vertexBufferIndex;
	meshData.IndexBuffer = indexBufferIndex;
	meshData.Mesh = mesh;
	meshData.Parts.reserve(mesh->BatchCount());

	auto batches = mesh->Batches();
	for (uint32_t batch = 0; batch < mesh->BatchCount(); ++batch)
	{
		MeshPart meshPart;
		meshPart.IndexStart = batches[batch].StartIndex;
		meshPart.IndexCount = batches[batch].IndexCount;
		meshPart.Visible = true;
		// Material not initialized
		meshData.Parts.push_back(move(meshPart));
	}

	_loadedFiles[fn] = meshData;
	_entityToIndex[entity] = static_cast<int>(_meshes.size());
	_meshes.push_back(move(meshData));
}

void StaticMeshManager::CreateStaticMesh(Entity entity, Mesh * mesh)
{
	if (!mesh)
	{
		TraceDebug("Tried to create mesh from empty mesh.");
		return;
	}

	uint32_t vertexBufferIndex = 0;
	uint32_t indexBufferIndex = 0;
	if (!_graphics.CreateMeshBuffers(mesh, vertexBufferIndex, indexBufferIndex))
	{
		SAFE_DELETE(mesh);
		TraceDebug("Failed to create buffers from mesh");

		return;
	}

	MeshData meshData;
	meshData.OwningEntity = entity;
	XMStoreFloat4x4(&meshData.Transform, XMMatrixIdentity());
	meshData.VertexBuffer = vertexBufferIndex;
	meshData.IndexBuffer = indexBufferIndex;
	meshData.Mesh = mesh;
	meshData.Parts.reserve(mesh->BatchCount());

	auto batches = mesh->Batches();
	for (uint32_t batch = 0; batch < mesh->BatchCount(); ++batch)
	{
		MeshPart meshPart;
		meshPart.IndexStart = batches[batch].StartIndex;
		meshPart.IndexCount = batches[batch].IndexCount;
		meshPart.Visible = true;
		// Material not initialized
		meshData.Parts.push_back(move(meshPart));
	}
	_entityToIndex[entity] = static_cast<int>(_meshes.size());
	_meshes.push_back(move(meshData));
}

void StaticMeshManager::CreateStaticMesh(Entity entity, const char * filename, std::vector<DirectX::XMFLOAT3>& pos, std::vector<DirectX::XMFLOAT2>& uvs, std::vector<uint>& indices)
{
	LPCSTR st = filename;
	string fn = PathFindFileNameA(st);

	auto get = _loadedFiles.find(fn);
	if (get != _loadedFiles.end())
	{
		MeshData meshData = get->second;
		meshData.OwningEntity = entity;
		XMStoreFloat4x4(&meshData.Transform, XMMatrixIdentity());

		for_each( meshData.Parts.begin(), meshData.Parts.end(), []( MeshPart &part )
		{
			part.Visible = true;
		} );

		_entityToIndex[entity] = static_cast<int>(_meshes.size());
		_meshes.push_back(move(meshData));
		TraceDebug("Tried to load model from data with occupied name.");
		return;

	}
	Mesh* mesh;
	try { mesh = new Mesh; }
	catch (ErrorMsg& msg)
	{
		throw msg;
	}

	mesh->AddAttributeStream(Mesh::AttributeType::Position, static_cast<unsigned int>(pos.size()), (float*)&pos[0], static_cast<unsigned int>(indices.size()), &indices[0]);
	mesh->AddAttributeStream(Mesh::AttributeType::TexCoord, static_cast<unsigned int>(uvs.size()), (float*)&uvs[0], static_cast<unsigned int>(indices.size()), &indices[0]);

	mesh->AddBatch(0, static_cast<unsigned int>(indices.size()));
	
	mesh->CalcNTB();
	uint32_t vertexBufferIndex = 0;
	uint32_t indexBufferIndex = 0;
	if (!_graphics.CreateMeshBuffers(mesh, vertexBufferIndex, indexBufferIndex))
	{
		SAFE_DELETE(mesh);
		TraceDebug("Failed to create buffers for file: '%s'", filename);

		return;
	}

	MeshData meshData;
	meshData.OwningEntity = entity;
	XMStoreFloat4x4(&meshData.Transform, XMMatrixIdentity());
	meshData.VertexBuffer = vertexBufferIndex;
	meshData.IndexBuffer = indexBufferIndex;
	meshData.Mesh = mesh;
	meshData.Parts.reserve(mesh->BatchCount());

	auto batches = mesh->Batches();
	for (uint32_t batch = 0; batch < mesh->BatchCount(); ++batch)
	{
		MeshPart meshPart;
		meshPart.IndexStart = batches[batch].StartIndex;
		meshPart.IndexCount = batches[batch].IndexCount;
		meshPart.Visible = true;
		// Material not initialized
		meshData.Parts.push_back(move(meshPart));
	}

	_loadedFiles[fn] = meshData;
	_entityToIndex[entity] = static_cast<int>(_meshes.size());
	_meshes.push_back(move(meshData));


}

void StaticMeshManager::CreateStaticMesh(Entity entity, const char * filename, std::vector<DirectX::XMFLOAT3>& pos, std::vector<DirectX::XMFLOAT2>& uvs, std::vector<uint>& indices, std::vector<SubMeshInfo> & subMeshInfo)
{
	LPCSTR st = filename;
	string fn = PathFindFileNameA(st);

	auto get = _loadedFiles.find(fn);
	if (get != _loadedFiles.end())
	{
		MeshData meshData = get->second;
		meshData.OwningEntity = entity;
		XMStoreFloat4x4(&meshData.Transform, XMMatrixIdentity());

		for_each(meshData.Parts.begin(), meshData.Parts.end(), [](MeshPart &part)
		{
			part.Visible = true;
		});

		_entityToIndex[entity] = static_cast<int>(_meshes.size());
		_meshes.push_back(move(meshData));
		TraceDebug("Tried to load model from data with occupied name.");
		return;

	}
	Mesh* mesh;
	try { mesh = new Mesh; }
	catch (ErrorMsg& msg)
	{
		throw msg;
	}

	mesh->AddAttributeStream(Mesh::AttributeType::Position,(uint) pos.size(), (float*)&pos[0], indices.size(), &indices[0]);
	mesh->AddAttributeStream(Mesh::AttributeType::TexCoord, (uint)uvs.size(), (float*)&uvs[0], indices.size(), &indices[0]);

	//mesh->AddBatch(0, indices.size());

	for (int i = 0; i < subMeshInfo.size(); i++)
	{
		mesh->AddBatch(subMeshInfo[i].indexStart, subMeshInfo[i].count);
	}

	mesh->CalcNTB();


	uint32_t vertexBufferIndex = 0;
	uint32_t indexBufferIndex = 0;
	if (!_graphics.CreateMeshBuffers(mesh, vertexBufferIndex, indexBufferIndex))
	{
		SAFE_DELETE(mesh);
		TraceDebug("Failed to create buffers for file: '%s'", filename);

		return;
	}

	MeshData meshData;
	meshData.OwningEntity = entity;
	XMStoreFloat4x4(&meshData.Transform, XMMatrixIdentity());
	meshData.VertexBuffer = vertexBufferIndex;
	meshData.IndexBuffer = indexBufferIndex;
	meshData.Mesh = mesh;
	meshData.Parts.reserve(mesh->BatchCount());

	auto batches = mesh->Batches();
	for (uint32_t batch = 0; batch < mesh->BatchCount(); ++batch)
	{
		MeshPart meshPart;
		meshPart.IndexStart = batches[batch].StartIndex;
		meshPart.IndexCount = batches[batch].IndexCount;
		meshPart.Visible = true;
		// Material not initialized
		meshData.Parts.push_back(move(meshPart));
	}

	_loadedFiles[fn] = meshData;
	_entityToIndex[entity] = static_cast<int>(_meshes.size());
	_meshes.push_back(move(meshData));
}

void StaticMeshManager::ReleaseMesh(Entity entity)
{
	auto got = _entityToIndex.find(entity);

	if (got == _entityToIndex.end())
	{
		TraceDebug("Tried to release nonexistant entity %d from StaticMeshManager.\n", entity.ID);
		return;
	}
	
	unsigned int index = got->second;
	MeshData& compare = _meshes[index];
	//Check if any other mesh has the same Mesh pointer
	//if it does, we dont want to delete the mesh
	int equalCount = -1; //Start at -1 since itself will always be equal
	for (auto &i : _meshes)
	{
		if (i.Mesh == compare.Mesh)
			++equalCount;
	}

	if (!equalCount)
	{
		//The same pointer might reside in _loadedFiles, gotta find it and erase it
		std::string name;
		for (auto &i : _loadedFiles)
		{
			if (i.second.Mesh == _meshes[index].Mesh)
				name = i.first;
		}
		SAFE_DELETE(_meshes[index].Mesh);
		_loadedFiles.erase(name);

		uint32_t vbIndex = _meshes[index].VertexBuffer;
		uint32_t ibIndex = _meshes[index].IndexBuffer;
		//The vertex- and index buffer must also be removed from the renderer
		//If they had the same Mesh-pointer, they will also have had the same
		//index to vertex- and index buffers.
		_graphics.ReleaseVertexBuffer(vbIndex);
		_graphics.ReleaseIndexBuffer(ibIndex);
		//After that's been done, now all the indices to vertex buffers and index buffers
		//greater than these two indices will be wrong, gotta fix that too
		for (auto &i : _meshes)
		{
			if (i.VertexBuffer > vbIndex)
				i.VertexBuffer--;
			if (i.IndexBuffer > ibIndex)
				i.IndexBuffer--;
		}
	}
	
	_entityToIndex.erase(entity);
	_meshes.erase(_meshes.begin() + index);
	//Now all the indices in _entityToIndex larger than got->second will be wrong
	//Iterate over the map and decrement
	for (auto &i : _entityToIndex)
	{
		if (i.second > index)
		{
			i.second--;
		}
	}
	
}


const Mesh * StaticMeshManager::GetMesh(const Entity & entity)
{
	auto meshIt = _entityToIndex.find(entity);

	if (meshIt != _entityToIndex.end())
	{

		return _meshes[meshIt->second].Mesh;
	}

	throw ErrorMsg(1200001, L"Tried to get mesh from an entity that had none.");
	return nullptr;
}

void StaticMeshManager::Hide( Entity entity, uint32_t subMesh )
{
	auto meshIt = _entityToIndex.find( entity );

	if ( meshIt != _entityToIndex.end() )
	{
		_meshes[meshIt->second].Parts[subMesh].Visible = false;
	}
}

void StaticMeshManager::Show( Entity entity, uint32_t subMesh )
{
	auto meshIt = _entityToIndex.find( entity );

	if ( meshIt != _entityToIndex.end() )
	{
		_meshes[meshIt->second].Parts[subMesh].Visible = true;
	}
}

void StaticMeshManager::ToggleVisibility( Entity entity, uint32_t subMesh )
{
	auto meshIt = _entityToIndex.find( entity );

	if ( meshIt != _entityToIndex.end() )
	{
		_meshes[meshIt->second].Parts[subMesh].Visible = !_meshes[meshIt->second].Parts[subMesh].Visible;
	}
}

const void StaticMeshManager::BindToRendered(bool exclusive)
{
	if (exclusive)
		System::GetGraphics()->ClearRenderProviders();
	System::GetGraphics()->AddRenderProvider(this);
	return void();
}

void StaticMeshManager::_TransformChanged( const Entity& entity, const XMMATRIX& transform, const XMVECTOR& pos, const XMVECTOR& dir, const XMVECTOR& up )
{
	auto meshIt = _entityToIndex.find( entity );

	if ( meshIt != _entityToIndex.end() )
	{
		// The entity has a mesh (we have an entry here)
		XMStoreFloat4x4( &_meshes[meshIt->second].Transform, transform );
	}
}

void StaticMeshManager::_MaterialChanged( const Entity& entity,const ShaderData* material, uint32_t subMesh)
{
	auto meshIt = _entityToIndex.find( entity );

	if ( meshIt != _entityToIndex.end() )
	{
		for (auto &i : _meshes[meshIt->second].Parts)
		{
			if (i.Material == nullptr)
			{
				i.Material = material;
				//i.Material.TextureCount = 0;
				//i.Material.Textures = nullptr;
				//i.Material.TextureOffsets.clear();
			}
		}
		if(subMesh < _meshes[meshIt->second].Parts.size())
			_meshes[meshIt->second].Parts[subMesh].Material = material;
	}
}

void StaticMeshManager::MaterialChanged(Entity entity, const ShaderData* material)
{
	auto meshIt = _entityToIndex.find(entity);

	if (meshIt != _entityToIndex.end())
	{
		for (auto &i : _meshes[meshIt->second].Parts)
		{
			i.Material = material;
		}
	}
}

void StaticMeshManager::_SetDefaultMaterials( const Entity& entity, const ShaderData* material)
{
	MeshData m = _meshes[_entityToIndex[entity]];
	for (auto &i : m.Parts)
	{
		i.Material = material;
	}
}
