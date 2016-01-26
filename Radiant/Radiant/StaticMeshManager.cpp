#include "StaticMeshManager.h"

#include "Utils.h"
#include "OBJLoader.h"

using namespace std;
using namespace DirectX;

StaticMeshManager::StaticMeshManager( Graphics& graphics/*, TransformManager& transformManager*/ ) :
	_graphics( graphics )
{
	_graphics.AddRenderProvider( this );

	//transformManager.SetTransformChangeCallback( [this]( Entity entity, const XMMATRIX& transform )
	//{
	//	TransformChanged( entity, transform );
	//} );
}

StaticMeshManager::~StaticMeshManager()
{
	for ( auto mesh : _meshes )
		SAFE_DELETE( mesh.Mesh );
}

void StaticMeshManager::GatherJobs( std::function</*const Material**/void(RenderJob&)> ProvideJob )
{
	RenderJob job;

	for ( auto& mesh : _meshes )
	{
		job.VertexBuffer = mesh.VertexBuffer;
		job.IndexBuffer = mesh.IndexBuffer;
		job.Transform = mesh.Transform;

		for ( auto& meshPart : mesh.Parts )
		{
			job.IndexStart = meshPart.IndexStart;
			job.IndexCount = meshPart.IndexCount;
			//job.Material = meshPart.Material;

			// Provide job, and if no material was set, a default one is
			// returned for us to use next time.
			//if ( const Material *newMat = ProvideJob( job ) )
			//	meshPart.Material = *newMat;
			ProvideJob( job );
		}
	}
}

void StaticMeshManager::CreateStaticMesh( Entity entity, const char *filename )
{
	OBJLoader loader;
	Mesh *mesh = loader.Load( filename );
	if ( !mesh )
	{
		TraceDebug( "Failed to load obj '%s'", filename );
		return;
	}

	//TraceDebug( "T-junctions found in %s: %d", filename, mesh->FixTJunctions() );
	mesh->FlipPositionZ();
	mesh->FlipNormals();
	mesh->InvertV();

	uint32_t vertexBufferIndex = 0;
	uint32_t indexBufferIndex = 0;
	if ( !_graphics.CreateBuffers( mesh, vertexBufferIndex, indexBufferIndex ) )
	{
		TraceDebug( "Failed to create buffers for file: '%s'", filename );
		SAFE_DELETE( mesh );
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

	_entityToIndex[entity] = _meshes.size();
	_meshes.push_back( move( meshData ) );
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
	//auto meshIt = mEntityToIndex.find( entity );

	//if ( meshIt != mEntityToIndex.end() )
	//{
	//	// The entity has a mesh (we have an entry here)
	//	XMStoreFloat4x4( &_Meshes[meshIt->second].Transform, transform );
	//}
}