#include "Lightning.h"

#include <algorithm>
#include <random>
#include "System.h"
#include "Utils.h"

using namespace std;
using namespace DirectX;

LightningManager::LightningManager( TransformManager& transformManager, MaterialManager& materialManager ) : _graphics( *System::GetGraphics() ), _materialManager( materialManager )
{
	_graphics.AddEffectProvider( this );

	transformManager.TransformChanged += Delegate<void( const Entity&, const XMMATRIX&, const XMVECTOR&, const XMVECTOR&, const XMVECTOR& )>::Make<LightningManager, &LightningManager::_TransformChanged>( this );
	materialManager.MaterialChanged += Delegate<void( const Entity&, const ShaderData*, int32_t )>::Make<LightningManager, &LightningManager::_MaterialChanged>( this );
	materialManager.MaterialCreated += Delegate<void( const Entity&, const ShaderData* )>::Make<LightningManager, &LightningManager::_MaterialCreated>( this );

	_generator  = default_random_engine( time( nullptr ) );
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

XMFLOAT3 operator+( const XMFLOAT3& v1, const XMFLOAT3& v2 )
{
	return XMFLOAT3( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z );
}

XMFLOAT3 operator-( const XMFLOAT3& v1, const XMFLOAT3& v2 )
{
	return XMFLOAT3( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z );
}

XMFLOAT3 operator*( float a, const XMFLOAT3& v )
{
	return XMFLOAT3( a * v.x, a * v.y, a * v.z );
}

XMFLOAT3 operator*( const XMFLOAT3& v, float a )
{
	return XMFLOAT3( a * v.x, a * v.y, a * v.z );
}

void LightningManager::CreateLightningBolt( Entity base, Entity target )
{
	uint32_t vb = _graphics.CreateDynamicVertexBuffer();
	TextureProxy sb = _graphics.CreateDynamicStructuredBuffer( sizeof( SegmentData ) );

	Bolt bolt;
	bolt.Base = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	bolt.Target = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	bolt.VertexBuffer = vb;
	bolt.SegmentBuffer = sb;
	bolt.Material = nullptr;

	_entityToIndex[base] = static_cast<int>(_bolts.size());
	_bolts.push_back( move( bolt ) );

	_materialManager.BindMaterial( base, "Shaders/LightningPS.hlsl" );
	_materialManager.SetMaterialProperty( base, "BoltColor", 1.0f, "Shaders/LightningPS.hlsl" );
	_materialManager.SetEntityTexture( base, "Segments", sb );
}

void LightningManager::BindToRenderer( bool exclusive )
{
	if ( exclusive )
		System::GetGraphics()->ClearEffectProviders();

	System::GetGraphics()->AddEffectProvider( this );
}

void LightningManager::Animate( const Entity& entity )
{
	auto it = _entityToIndex.find( entity );
	if ( it == _entityToIndex.end() )
		return;

	Bolt& bolt = _bolts[it->second];

	// Generate orthogonal base to vector depending on which coordinate
	// is the smallest. Index on x, y, and z respectively.
	void( *OrthoBase[3] )(const XMFLOAT3&, XMFLOAT3&, XMFLOAT3&) =
	{
		// First coord is minimal
		[]( const XMFLOAT3& v, XMFLOAT3& a, XMFLOAT3& b ) -> void
		{
			a = XMFLOAT3( 0, -v.z, v.y ); // dot(a, v) == 0
			b = XMFLOAT3( v.y * v.y + v.z * v.z, -v.x * v.y, -v.x * v.z ); // dot(a, b) == 0, dot(a, b) == 0
		},
		// Second coord is minimal
		[]( const XMFLOAT3& v, XMFLOAT3& a, XMFLOAT3& b ) -> void
		{
			a = XMFLOAT3( -v.z, 0, v.x ); // dot(a, v) == 0
			b = XMFLOAT3( -v.y * v.x, v.x * v.x + v.z * v.z, -v.y * v.z ); // dot(a, b) == 0, dot(a, b) == 0
		},
		// Third coord is minimal
		[]( const XMFLOAT3& v, XMFLOAT3& a, XMFLOAT3& b ) -> void
		{
			a = XMFLOAT3( -v.y, v.x, 0 ); // dot(a, v) == 0
			b = XMFLOAT3( -v.z * v.x, -v.z * v.y, v.x * v.x + v.y * v.y ); // dot(a, b) == 0, dot(a, b) == 0
		},
	};

	// v: normal vector, m: mid point, r: max disc width
	auto PointAroundVec = [&OrthoBase, this]( const XMFLOAT3& v, const XMFLOAT3& m, float maxRange ) -> XMFLOAT3
	{
		// Generate orthogonal basis vectors u,w to v. http://stackoverflow.com/questions/19337314/generate-random-point-on-a-2d-disk-in-3d-space-given-normal-vector
		float absolutes[] = { fabsf( v.x ), fabsf( v.y ), fabsf( v.z ) };

		uint32_t minIndex = 0;
		if ( absolutes[1] <= absolutes[0] && absolutes[1] <= absolutes[2] )
			minIndex = 1;
		else if ( absolutes[2] <= absolutes[0] && absolutes[2] <= absolutes[1] )
			minIndex = 2;

		// We have index of minimal component, use this to call the correct function
		// to generate basis vectors a and b.
		XMFLOAT3 a, b;
		OrthoBase[minIndex]( v, a, b );

		XMVECTOR normA = XMVector3NormalizeEst( XMLoadFloat3( &a ) );
		XMVECTOR normB = XMVector3NormalizeEst( XMLoadFloat3( &b ) );

		// Randomize polar r, psi
		uniform_real_distribution<float> rDist( 0.5f * maxRange, maxRange );
		uniform_real_distribution<float> phiDist( 0.0f, XM_2PI );
		float r = rDist( _generator );
		float phi = phiDist( _generator );

		// Convert to cartesian, offset basis vectors from m.
		XMVECTOR point = XMLoadFloat3( &m ) + normA * r * cosf( phi ) + normB * r * sinf( phi );
		XMFLOAT3 retVal;
		XMStoreFloat3( &retVal, point );
		return retVal;
	};

	// http://drilian.com/2009/02/25/lightning-bolts/

	XMFLOAT3 start( bolt.Base );
	XMFLOAT3 end( bolt.Target );
	XMFLOAT3 mid = (start + end) * 0.5f;
	bolt.Segments.clear();
	bolt.Segments.push_back( Segment( start, end ) );
	SegmentData segmentData;
	segmentData.Intensity = 1.0f;
	segmentData.Color = XMFLOAT3( 1.0f, 0.0f, 0.0f );
	bolt.SegmentData.clear();
	bolt.SegmentData.push_back( segmentData );

	float maxOffset = 3.0f;
	int generations = 5;

	for ( int gen = 0; gen < generations; ++gen )
	{
		for ( auto it = bolt.Segments.begin(); it != bolt.Segments.end(); /*empty*/ )
		{
			start = it->Start;
			end = it->End;
			mid = (start + end) * 0.5f;

			// Get a point in the plane containing mid with normal parallell to the segment.
			mid = PointAroundVec( end - mid, mid, maxOffset );

			uint32_t index = it - bolt.Segments.begin();
			auto segDataIt = bolt.SegmentData.begin() + index;
			segmentData = *segDataIt;

			// Split the segment into two, connected via the offset midpoint.
			it = bolt.Segments.erase( it );
			segDataIt = bolt.SegmentData.erase( segDataIt );
			it = bolt.Segments.emplace( it, Segment( start, mid ) ) + 1;
			segDataIt = bolt.SegmentData.emplace( segDataIt, segmentData ) + 1;
			it = bolt.Segments.emplace( it, Segment( mid, end ) ) + 1;
			segDataIt = bolt.SegmentData.emplace( segDataIt, segmentData ) + 1;
			
			segmentData.Intensity *= 0.3f; // Reduce intensity for branches
			//segmentData.Color = XMFLOAT3( 0.0f, 1.0f, 0.0f );

			uniform_real_distribution<float> branchDist( 0.0f, 1.0f );
			bool branch = branchDist( _generator ) < 0.9f - gen * 0.2f; // Probability w.r.t generation

			if ( branch )
			{
				it = bolt.Segments.emplace( it, Segment( mid, mid + (mid - start) * 0.7f ) ) + 1;
				segDataIt = bolt.SegmentData.emplace( segDataIt, segmentData ) + 1;
			}
		}

		maxOffset *= 0.5f; // Next generation may just offset half as much.
	}

	//auto it = bolt.Segments.begin();
	//start = it->Start;
	//end = it->End;
	//mid = (start + end) * 0.5f;
	//for ( int g = 0; g < 100; ++g )
	//{
	//	XMFLOAT3 hej( PointAroundVec( end - mid, mid, maxOffset ) );
	//	bolt.Segments.push_back( Segment( mid, hej ) );
	//}

	_graphics.UpdateDynamicVertexBuffer( bolt.VertexBuffer, bolt.Segments.data(), sizeof( Segment ) * bolt.Segments.size() );
	_graphics.UpdateDynamicStructuredBuffer( bolt.SegmentBuffer, bolt.SegmentData.data(), sizeof( SegmentData ), bolt.SegmentData.size() );
}

void LightningManager::_TransformChanged( const Entity& entity, const XMMATRIX& transform, const XMVECTOR& pos, const XMVECTOR& dir, const XMVECTOR& up )
{
	auto baseIt = _entityToIndex.find( entity );

	if ( baseIt != _entityToIndex.end() )
	{
		XMStoreFloat3( &_bolts[baseIt->second].Base, pos );
	}

	auto range = _targetToIndices.equal_range( entity );
	for ( auto it = range.first; it != range.second; ++it )
	{
		// Use it->second (index) to get the bolt for which the target has changed.
		XMStoreFloat3( &_bolts[it->second].Target, pos );
	}
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
