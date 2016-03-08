#include "ProximityLightning.h"

using namespace std;
using namespace DirectX;

ProximityLightningManager::ProximityLightningManager( TransformManager& transform, LightningManager& lightning ) : _transformManager( transform ), _lightningManager( lightning )
{
	_transformManager.TransformChanged += Delegate<void( const Entity&, const XMMATRIX&, const XMVECTOR&, const XMVECTOR&, const XMVECTOR& )>::Make<ProximityLightningManager, &ProximityLightningManager::_TransformChanged>( this );
}

ProximityLightningManager::~ProximityLightningManager()
{

}

void ProximityLightningManager::Add( const Entity& entity )
{
	ProximityLightningData data;
	data.Position = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );
	_proximityBolts[entity] = data;
}

void ProximityLightningManager::Remove( const Entity& entity )
{
	_lightningManager.Remove( entity );
	_proximityBolts.erase( entity );
}

void ProximityLightningManager::Update()
{
	for ( auto base : _proximityBolts )
	{
		_lightningManager.Remove( base.first );
	}

	for ( auto base : _proximityBolts )
	{
		for ( auto target : _proximityBolts )
		{
			if ( base.first == target.first )
				continue;

			if ( XMVectorGetX( XMVector3LengthSq( base.second.Position - target.second.Position ) ) <= 3.0f * 3.0f )
			{
				// Lightning back comes later in the loop ofc
				_lightningManager.CreateLightningBolt( base.first, target.first );
				_lightningManager.Animate( base.first );
			}
		}
	}
}

void ProximityLightningManager::_TransformChanged( const Entity& entity, const XMMATRIX& transform, const XMVECTOR& pos, const XMVECTOR& dir, const XMVECTOR& up )
{
	auto baseIt = _proximityBolts.find( entity );

	if ( baseIt != _proximityBolts.end() )
	{
		baseIt->second.Position = pos;
	}
}
