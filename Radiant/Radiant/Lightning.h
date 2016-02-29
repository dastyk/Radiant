#ifndef _LIGHTNING_H_
#define _LIGHTNING_H_

#include "IEffectProvider.h"

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
#include <random>
#include "Graphics.h"
#include "Entity.h"
#include "MaterialManager.h"
#include <map>
#include "ShaderData.h"
#include "TransformManager.h"

class LightningManager : public IEffectProvider
{
public:
	LightningManager( TransformManager& transformManager, MaterialManager& materialManager );
	~LightningManager();

	void CreateLightningBolt( Entity base, Entity target );

	void GatherEffects( std::vector<Effect>& effects );

	void BindToRenderer( bool exclusive );

	void Animate( const Entity& entity );

private:
	struct SegmentData
	{
		float Intensity;
		DirectX::XMFLOAT3 Color;
	};

	struct Segment
	{
		Segment( DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end ) : Start( start ), End( end ) {}

		DirectX::XMFLOAT3 Start;
		DirectX::XMFLOAT3 End;
	};

	struct Bolt
	{
		DirectX::XMFLOAT3 Base;
		DirectX::XMFLOAT3 Target;
		std::uint32_t VertexBuffer;
		TextureProxy SegmentBuffer;
		const ShaderData *Material;
		std::vector<Segment> Segments;
		std::vector<SegmentData> SegmentData;
	};

private:
	void _TransformChanged( const Entity& entity, const DirectX::XMMATRIX& transform, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& up );
	void _MaterialChanged( const Entity& entity, const ShaderData* material, std::int32_t subMesh );
	void _MaterialCreated( const Entity& entity, const ShaderData* material );

private:
	std::vector<Bolt> _bolts;
	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
	std::unordered_multimap<Entity, std::uint32_t, EntityHasher> _targetToIndices;
	Graphics& _graphics;
	MaterialManager& _materialManager;
	std::default_random_engine _generator;
};

#endif