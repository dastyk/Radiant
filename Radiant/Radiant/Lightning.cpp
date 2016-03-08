#include "Lightning.h"

#include <algorithm>
#include <random>
#include "System.h"
#include "Utils.h"

using namespace std;
using namespace DirectX;

LightningManager::LightningManager( TransformManager& transformManager, MaterialManager& materialManager ) : _graphics( *System::GetGraphics() ), _transformManager( transformManager ), _materialManager( materialManager )
{
	_graphics.AddEffectProvider( this );

	transformManager.TransformChanged += Delegate<void( const Entity&, const XMMATRIX&, const XMVECTOR&, const XMVECTOR&, const XMVECTOR& )>::Make<LightningManager, &LightningManager::_TransformChanged>( this );
	materialManager.MaterialChanged += Delegate<void( const Entity&, const ShaderData*, int32_t )>::Make<LightningManager, &LightningManager::_MaterialChanged>( this );
	materialManager.MaterialCreated += Delegate<void( const Entity&, const ShaderData* )>::Make<LightningManager, &LightningManager::_MaterialCreated>( this );

	_generator = default_random_engine( time( nullptr ) );

	_randomColors = new XMFLOAT3[_randomColorCount];

	uniform_real_distribution<float> dist( 0.2f, 1.0f );
	for ( int i = 0; i < _randomColorCount; ++i )
	{
		_randomColors[i] = XMFLOAT3( dist( _generator ), dist( _generator ), dist( _generator ) );
	}

	for ( int i = 0; i < 50; ++i )
	{
		_CreateLightningBolt();
	}
}

LightningManager::~LightningManager()
{
	for ( auto& bolt : _pregeneratedBuffers )
	{
		// sb released by material
		_graphics.ReleaseDynamicVertexBuffer( get<0>( bolt ) );
	}

	SAFE_DELETE_ARRAY( _randomColors );
}

void LightningManager::GatherEffects( vector<Effect>& effects )
{
	for ( auto& bolt : _entityToBolt )
	{
		effects.push_back( Effect() );
		Effect& e = effects.back();
		e.Material = bolt.second.Material;
		e.VertexBuffer = bolt.second.VertexBuffer;
		e.VertexCount = bolt.second.VertexCount;

		XMVECTOR basePos = _transformManager.GetPositionW( bolt.second.Base );
		XMVECTOR targetPos = _transformManager.GetPositionW( bolt.second.Target );
		float scaleZ = XMVectorGetX( XMVector3Length( targetPos - basePos ) );

		XMVECTOR dir = (targetPos - basePos) / scaleZ;
		XMVECTOR cross = XMVector3Cross( XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ), dir );
		float angle = XMVectorGetX( XMVector3Dot( XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ), dir ) );
		angle = acosf( angle );

		XMVECTOR scale = _transformManager.GetScale( bolt.second.Base );
		bolt.second.ScaleXY = XMFLOAT2( XMVectorGetX( scale ), XMVectorGetY( scale ) );

		e.World = XMMatrixScaling( bolt.second.ScaleXY.x, bolt.second.ScaleXY.y, scaleZ ) * XMMatrixRotationNormal( cross, angle ) * XMMatrixTranslationFromVector( basePos );
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
	uniform_int_distribution<int> randomBoltDist( 0, 49 );
	auto& pregenerated = _pregeneratedBuffers[randomBoltDist( _generator )];

	Bolt bolt;
	bolt.Base = base;
	bolt.Target = target;
	bolt.VertexBuffer = get<0>( pregenerated );
	bolt.VertexCount = get<1>( pregenerated );
	bolt.SegmentBuffer = get<2>( pregenerated );
	bolt.Material = nullptr;
	bolt.RainbowSith = false;

	_entityToBolt[base] = bolt;

	_materialManager.BindMaterial( base, "Shaders/LightningPS.hlsl" );
	_materialManager.SetMaterialProperty( base, "BoltColor", 1.0f, "Shaders/LightningPS.hlsl" );
	_materialManager.SetEntityTexture( base, "Segments", bolt.SegmentBuffer );
}

void LightningManager::_CreateLightningBolt()
{
	uint32_t vb = _graphics.CreateDynamicVertexBuffer();
	TextureProxy sb = _graphics.CreateDynamicStructuredBuffer( sizeof( SegmentData ) );

	_pregeneratedBuffers.push_back( make_tuple( vb, 0, sb ) );
	_Animate( _pregeneratedBuffers.size() - 1, false );
}

void LightningManager::Remove( const Entity& e )
{
	auto it = _entityToBolt.find( e );
	if ( it == _entityToBolt.end() )
		return;

	Entity target = it->second.Target;
	_entityToBolt.erase( it->first );

	_materialManager.ReleaseMaterial( e );
}

void LightningManager::BindToRenderer( bool exclusive )
{
	if ( exclusive )
		System::GetGraphics()->ClearEffectProviders();

	System::GetGraphics()->AddEffectProvider( this );
}

void LightningManager::Animate( const Entity& entity )
{
	auto it = _entityToBolt.find( entity );
	if ( it == _entityToBolt.end() )
		return;

	Bolt& bolt = it->second;
	uniform_int_distribution<int> randomBoltDist( 0, 49 );
	auto& pregenerated = _pregeneratedBuffers[randomBoltDist( _generator )];
	bolt.VertexBuffer = get<0>( pregenerated );
	bolt.VertexCount = get<1>( pregenerated );
	bolt.SegmentBuffer = get<2>( pregenerated );
}

void LightningManager::_Animate( uint32_t index, bool rainbowSith )
{
	auto& bolt = _pregeneratedBuffers[index];

	// m: mid point, r: max disc width
	auto PointOnDisc = [this]( const XMFLOAT3& m, float maxRange ) -> XMFLOAT3
	{
		// Randomize polar r, psi
		uniform_real_distribution<float> rDist( 0.5f * maxRange, maxRange );
		uniform_real_distribution<float> phiDist( 0.0f, XM_2PI );
		float r = rDist( _generator );
		float phi = phiDist( _generator );

		// Convert to cartesian, offset basis vectors from m.
		XMVECTOR point = XMLoadFloat3( &m ) + XMVectorSet( r * cosf( phi ), r * sinf( phi ), 0.0f, 0.0f );
		XMFLOAT3 retVal;
		XMStoreFloat3( &retVal, point );
		return retVal;
	};

	// http://drilian.com/2009/02/25/lightning-bolts/

	static std::vector<Segment> segments;
	static vector<SegmentData> segmentDataVector;

	segments.clear();
	segmentDataVector.clear();

	XMFLOAT3 start( 0.0f, 0.0f, 0.0f );
	XMFLOAT3 end( 0.0f, 0.0f, 1.0f );
	XMFLOAT3 mid = (start + end) * 0.5f;
	segments.push_back( Segment( start, end ) );
	SegmentData segmentData;
	segmentData.Intensity = 100.0f;
	segmentData.Color = XMFLOAT3( 148.0f / 255.0f, 0.0f, 1.0f );
	segmentDataVector.push_back( segmentData );

	float maxOffset = 0.5f;
	int generations = 5;

	uniform_int_distribution<int> randomColorDist( 0, _randomColorCount - 1 );

	for ( int gen = 0; gen < generations; ++gen )
	{
		for ( auto it = segments.begin(); it != segments.end(); /*empty*/ )
		{
			start = it->Start;
			end = it->End;
			mid = (start + end) * 0.5f;

			// Get a point in the plane containing mid with normal parallell to the z-axis.
			mid = PointOnDisc( mid, maxOffset );

			uint32_t index = it - segments.begin();
			auto segDataIt = segmentDataVector.begin() + index;
			segmentData = *segDataIt;

			// Split the segment into two, connected via the offset midpoint.
			it = segments.erase( it );
			segDataIt = segmentDataVector.erase( segDataIt );
			it = segments.emplace( it, Segment( start, mid ) ) + 1;
			if ( rainbowSith )
				segmentData.Color = _randomColors[randomColorDist( _generator )];
			segDataIt = segmentDataVector.emplace( segDataIt, segmentData ) + 1;
			it = segments.emplace( it, Segment( mid, end ) ) + 1;
			if ( rainbowSith )
				segmentData.Color = _randomColors[randomColorDist( _generator )];
			segDataIt = segmentDataVector.emplace( segDataIt, segmentData ) + 1;

			segmentData.Intensity *= 0.3f; // Reduce intensity for branches
			if ( rainbowSith )
				segmentData.Color = _randomColors[randomColorDist( _generator )];

			uniform_real_distribution<float> branchDist( 0.0f, 1.0f );
			bool branch = branchDist( _generator ) < 0.3f - gen * 0.2f; // Probability w.r.t generation

			if ( branch )
			{
				it = segments.emplace( it, Segment( mid, mid + (mid - start) * 0.7f ) ) + 1;
				segDataIt = segmentDataVector.emplace( segDataIt, segmentData ) + 1;
			}
		}

		maxOffset *= 0.5f; // Next generation may just offset half as much.
	}

	get<1>( bolt ) = segments.size() * 2;
	_graphics.UpdateDynamicVertexBuffer( get<0>( bolt ), segments.data(), sizeof( Segment ) * segments.size() );
	_graphics.UpdateDynamicStructuredBuffer( get<2>( bolt ), segmentDataVector.data(), sizeof( SegmentData ), segmentDataVector.size() );
}

void LightningManager::SetRainbowSith( const Entity& entity, bool sith )
{
	auto it = _entityToBolt.find( entity );

	if ( it != _entityToBolt.end() )
	{
		it->second.RainbowSith = sith;
	}
}

void LightningManager::_TransformChanged( const Entity& entity, const XMMATRIX& transform, const XMVECTOR& pos, const XMVECTOR& dir, const XMVECTOR& up )
{
	auto baseIt = _entityToBolt.find( entity );
	if ( baseIt != _entityToBolt.end() )
	{
		XMVECTOR scale, rot, tran;
		XMMatrixDecompose( &scale, &rot, &tran, transform );

		// Perhaps this could be aquired on gather like position
		baseIt->second.ScaleXY = XMFLOAT2( XMVectorGetX( scale ), XMVectorGetY( scale ) );
	}
}

void LightningManager::_MaterialChanged( const Entity& entity, const ShaderData* material, int32_t subMesh )
{
	auto boltIt = _entityToBolt.find( entity );
	if ( boltIt != _entityToBolt.end() )
	{
		boltIt->second.Material = material;
	}
}

void LightningManager::_MaterialCreated( const Entity& entity, const ShaderData* material )
{
	_MaterialChanged( entity, material, -1 );
}
