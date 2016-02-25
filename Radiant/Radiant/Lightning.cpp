#include "Lightning.h"

#include <algorithm>
#include "System.h"
#include "Utils.h"

using namespace std;
using namespace DirectX;

/*
Idea: dynamic vertex buffer from renderer.
Material will need to use a vertex shader
Need job to use line render
Put positions in dynamic vertex buffer
Set vertex count
Vertex shader pretty much just outputs position
Material shader outputs to glow

In graphics: use speciul vertex buffer, use lines for effects and no index buffer for now
Also render without depth test. Do this after GBuffer stuff for effects. Can hardcode rendering
to emissive for now.

Just generate a straight line for now. Add subdivision and animate it later.
*/

LightningManager::LightningManager( TransformManager& transformManager, MaterialManager& materialManager ) : _graphics( *System::GetGraphics() ), _materialManager( materialManager )
{
	_graphics.AddEffectProvider( this );

	transformManager.TransformChanged += Delegate<void( const Entity&, const XMMATRIX&, const XMVECTOR&, const XMVECTOR&, const XMVECTOR& )>::Make<LightningManager, &LightningManager::_TransformChanged>( this );
	materialManager.MaterialChanged += Delegate<void( const Entity&, const ShaderData*, int32_t )>::Make<LightningManager, &LightningManager::_MaterialChanged>( this );
	materialManager.MaterialCreated += Delegate<void( const Entity&, const ShaderData* )>::Make<LightningManager, &LightningManager::_MaterialCreated>( this );
}

LightningManager::~LightningManager()
{
	for ( auto& bolt : _bolts )
	{
		_graphics.ReleaseDynamicVertexBuffer( bolt.VertexBuffer );
	}
}

void LightningManager::GatherEffects( vector<Effect>& effects )
{
	for ( auto& bolt : _bolts )
	{
		effects.push_back( Effect() );
		Effect& e = effects.back();
		e.Material = bolt.Material;
		e.VertexBuffer = bolt.VertexBuffer;
		e.VertexCount = bolt.Segments.size() * 2;
	}
}

void LightningManager::CreateLightningBolt( Entity base, Entity target )
{
	uint32_t vb = _graphics.CreateDynamicVertexBuffer();

	Bolt bolt;
	//bolt.Base;
	//bolt.Target;
	bolt.VertexBuffer = vb;
	bolt.Material = nullptr;
	
	Segment seg;
	seg.Start = XMFLOAT3( 25, 3, 25 );
	seg.End = XMFLOAT3( 26, 3, 26 );
	bolt.Segments.push_back( move( seg ) );
	seg.Start = XMFLOAT3( 26, 3, 26 );
	seg.End = XMFLOAT3( 25.5, 2, 25.5 );
	bolt.Segments.push_back( move( seg ) );

	_graphics.UpdateDynamicVertexBuffer( vb, bolt.Segments.data(), sizeof( Segment ) * bolt.Segments.size() );

	_entityToIndex[base] = static_cast<int>(_bolts.size());
	_bolts.push_back( move( bolt ) );

	_materialManager.BindMaterial( base, "Shaders/LightningPS.hlsl" );
	_materialManager.SetMaterialProperty( base, "BoltColor", 1.0f, "Shaders/LightningPS.hlsl" );
}

void LightningManager::BindToRenderer( bool exclusive )
{
	if ( exclusive )
		System::GetGraphics()->ClearEffectProviders();

	System::GetGraphics()->AddEffectProvider( this );
}

void LightningManager::_TransformChanged( const Entity& entity, const XMMATRIX& transform, const XMVECTOR& pos, const XMVECTOR& dir, const XMVECTOR& up )
{
	//auto meshIt = _entityToIndex.find( entity );

	//if ( meshIt != _entityToIndex.end() )
	//{
	//	// The entity has a mesh (we have an entry here)
	//	XMStoreFloat4x4( &_meshes[meshIt->second].Transform, transform );
	//}
}

void LightningManager::_MaterialChanged( const Entity& entity, const ShaderData* material, int32_t subMesh )
{
	auto boltIt = _entityToIndex.find( entity );

	if ( boltIt != _entityToIndex.end() )
	{
		_bolts[boltIt->second].Material = material;
	}
}

void LightningManager::_MaterialCreated( const Entity& entity, const ShaderData* material )
{
	_MaterialChanged( entity, material, -1 );
}
