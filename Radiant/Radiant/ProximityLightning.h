#ifndef _PROXIMITY_LIGHTNING_H_
#define _PROXIMITY_LIGHTNING_H_

#include <map>
#include <DirectXMath.h>
#include "TransformManager.h"
#include "Lightning.h"
#include "Entity.h"

class ProximityLightningManager
{
public:
	ProximityLightningManager( TransformManager& transform, LightningManager& lightning );
	~ProximityLightningManager();

	void Add( const Entity& entity );
	void Remove( const Entity& entity );
	void Update();

private:
	struct ProximityLightningData
	{
		ProximityLightningData() : Position( DirectX::XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f ) ) {}
		DirectX::XMVECTOR Position;
	};

private:
	void _TransformChanged( const Entity& entity, const DirectX::XMMATRIX& transform, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& right, const DirectX::XMVECTOR& up );

private:
	TransformManager& _transformManager;
	LightningManager& _lightningManager;
	std::unordered_map<Entity, ProximityLightningData, EntityHasher> _proximityBolts;
};

#endif
