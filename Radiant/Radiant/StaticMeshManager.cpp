#include "StaticMeshManager.h"
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

	
	materialManager.SetMaterialChangeCallback([this](Entity entity, const ShaderData& material, uint32_t subMesh)
	{
		MaterialChanged(entity, material, subMesh);
	});

	materialManager.SetMaterialCreatedCallback([this](Entity entity, const ShaderData& material)
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
			j.push_back(RenderJob(meshPart.IndexStart, meshPart.IndexCount, meshPart.Material));
			//tRJ.IndexStart = meshPart.IndexStart;
			//tRJ.IndexCount = meshPart.IndexCount;
			//job.Material = meshPart.Material;

			// Provide job, and if no material was set, a default one is
			// returned for us to use next time.
			//if ( const Material *newMat = ProvideJob( job ) )
			//	meshPart.Material = *newMat;
			
		}
	}
}
void StaticMeshManager::CreateStaticMesh( Entity entity, const char *filename )
{
	LPCSTR st = filename;
	string fn = PathFindFileNameA(st);

	auto get = _loadedFiles.find(fn);
	if(get != _loadedFiles.end())
	{
			MeshData meshData = get->second;
			meshData.OwningEntity = entity;
			XMStoreFloat4x4(&meshData.Transform, XMMatrixIdentity());

			_entityToIndex[entity] = static_cast<int>(_meshes.size());
			_meshes.push_back(move(meshData));
			return;

	}

	Mesh *mesh;
	try{ mesh = System::GetFileHandler()->LoadModel(filename); }
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
	if ( !_graphics.CreateMeshBuffers( mesh, vertexBufferIndex, indexBufferIndex ) )
	{
		SAFE_DELETE(mesh);
		TraceDebug( "Failed to create buffers for file: '%s'", filename );
		
		return;
	}

	MeshData meshData;
	meshData.OwningEntity = entity;
	XMStoreFloat4x4( &meshData.Transform, XMMatrixIdentity() );
	meshData.VertexBuffer = vertexBufferIndex;
	meshData.IndexBuffer = indexBufferIndex;
	meshData.Mesh = mesh;
	meshData.Parts.reserve( mesh->BatchCount() );

	auto batches = mesh->Batches();
	for ( uint32_t batch = 0; batch < mesh->BatchCount(); ++batch )
	{
		MeshPart meshPart;
		meshPart.IndexStart = batches[batch].StartIndex;
		meshPart.IndexCount = batches[batch].IndexCount;
		// Material not initialized
		meshData.Parts.push_back( move( meshPart ) );
	}

	_loadedFiles[fn] = meshData;
	_entityToIndex[entity] = static_cast<int>(_meshes.size());
	_meshes.push_back( move( meshData ) );
}

const void StaticMeshManager::BindToRendered(bool exclusive)
{
	if (exclusive)
		System::GetGraphics()->ClearRenderProviders();
	System::GetGraphics()->AddRenderProvider(this);
	return void();
}

//Material& StaticMeshManager::GetMaterial( Entity entity, uint32_t part )
//{
//	auto indexIt = mEntityToIndex.find( entity );
//
//	if ( indexIt != mEntityToIndex.end() )
//	{
//		return _Meshes[indexIt->second].Parts[part].Material;
//	}
//
//	throw;
//}
//
//void StaticMeshManager::SetMaterial( Entity entity, std::uint32_t part, const Material& material )
//{
//	auto indexIt = mEntityToIndex.find( entity );
//
//	if ( indexIt != mEntityToIndex.end() )
//	{
//		_Meshes[indexIt->second].Parts[part].Material = material;
//	}
//}

void StaticMeshManager::TransformChanged( Entity entity, const XMMATRIX& transform )
{
	auto meshIt = _entityToIndex.find( entity );

	if ( meshIt != _entityToIndex.end() )
	{
		// The entity has a mesh (we have an entry here)
		XMStoreFloat4x4( &_meshes[meshIt->second].Transform, transform );
	}
}

void StaticMeshManager::MaterialChanged(Entity entity, const ShaderData& material, uint32_t subMesh)
{
	auto meshIt = _entityToIndex.find( entity );

	if ( meshIt != _entityToIndex.end() )
	{
		for (auto &i : _meshes[meshIt->second].Parts)
		{
			if (i.Material.Shader == -1)
			{
				i.Material = material;
				i.Material.TextureCount = 0;
				i.Material.Textures = nullptr;
				i.Material.TextureOffsets.clear();
			}
		}
		_meshes[meshIt->second].Parts[subMesh].Material = material;
	}
}

void StaticMeshManager::_SetDefaultMaterials(Entity entity, const ShaderData & material)
{
	MeshData m = _meshes[_entityToIndex[entity]];
	for (auto &i : m.Parts)
	{
		i.Material = material;
	}
}
