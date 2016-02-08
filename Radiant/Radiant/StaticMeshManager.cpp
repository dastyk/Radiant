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
	

	transformManager.SetTransformChangeCallback( [this]( Entity entity, const XMMATRIX& transform )
	{
		TransformChanged( entity, transform );
	} );

	
	materialManager.SetMaterialChangeCallback([this](Entity entity,const ShaderData* material, uint32_t subMesh)
	{
		MaterialChanged(entity, material, subMesh);
	});

	materialManager.SetMaterialCreatedCallback([this](Entity entity,const ShaderData* material)
	{
		auto find = _entityToIndex.find(entity);
		if (find != _entityToIndex.end())
		{
			_SetDefaultMaterials(entity, material);
		}
	});

	materialManager.GetSubMeshCount([this](Entity entity)
	{
		if(_entityToIndex.count(entity))
			return static_cast<uint32_t>(_meshes[_entityToIndex[entity]].Parts.size());
		return static_cast<uint32_t>(0);//If its not a mesh, return 0
	});
}

StaticMeshManager::~StaticMeshManager()
{
	for ( auto file : _loadedFiles )
		SAFE_DELETE(file.second.Mesh);

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
		RenderJobMap4& j = jobs[mesh.VertexBuffer][mesh.IndexBuffer][(void*)&mesh.Transform];

		for (auto& meshPart : mesh.Parts)
		{
			if ( meshPart.Visible )
				j.push_back( &meshPart ); 
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

	mesh->AddAttributeStream(Mesh::AttributeType::Position, pos.size(), (float*)&pos[0], indices.size(), &indices[0]);
	mesh->AddAttributeStream(Mesh::AttributeType::TexCoord, uvs.size(), (float*)&uvs[0], indices.size(), &indices[0]);

	mesh->AddBatch(0, indices.size());
	
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

	mesh->AddAttributeStream(Mesh::AttributeType::Position, pos.size(), (float*)&pos[0], indices.size(), &indices[0]);
	mesh->AddAttributeStream(Mesh::AttributeType::TexCoord, uvs.size(), (float*)&uvs[0], indices.size(), &indices[0]);

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
	//Flöjt vet hur mesharna funkar, han för göra det.
	
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

void StaticMeshManager::TransformChanged( Entity entity, const XMMATRIX& transform )
{
	auto meshIt = _entityToIndex.find( entity );

	if ( meshIt != _entityToIndex.end() )
	{
		// The entity has a mesh (we have an entry here)
		XMStoreFloat4x4( &_meshes[meshIt->second].Transform, transform );
	}
}

void StaticMeshManager::MaterialChanged(Entity entity,const ShaderData* material, uint32_t subMesh)
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

void StaticMeshManager::_SetDefaultMaterials(Entity entity, const ShaderData* material)
{
	MeshData m = _meshes[_entityToIndex[entity]];
	for (auto &i : m.Parts)
	{
		i.Material = material;
	}
}
