#ifndef _LIGHTNING_H_
#define _LIGHTNING_H_

#include "IEffectProvider.h"

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
#include <random>
#include <stack>
#include <tuple>
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
	void Remove( const Entity& e );

	void GatherEffects( std::vector<Effect>& effects );

	void BindToRenderer( bool exclusive );

	void Animate( const Entity& entity );

	void SetRainbowSith( const Entity& entity, bool sith );

	void SetScaleX( const Entity& entity, float scale );
	void SetScaleY( const Entity& entity, float scale );
	void SetScale( const Entity& entity, const DirectX::XMFLOAT2& scale );
	DirectX::XMFLOAT2 GetScale( const Entity& entity );

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
		Entity Base;
		Entity Target;
		std::uint32_t VertexBuffer;
		TextureProxy SegmentBuffer;
		const ShaderData *Material;
		std::vector<Segment> Segments;
		std::vector<SegmentData> SegmentData;
		bool RainbowSith;
		DirectX::XMFLOAT2 ScaleXY;
		std::uint32_t VertexCount;
	};

private:
	void _TransformChanged( const Entity& entity, const DirectX::XMMATRIX& transform, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& right, const DirectX::XMVECTOR& up );
	void _MaterialChanged( const Entity& entity, const ShaderData* material, std::int32_t subMesh );
	void _MaterialCreated( const Entity& entity, const ShaderData* material );
	void _CreateLightningBolt( void );
	void _Animate( std::uint32_t index, bool rainbowSith );

private:
	std::unordered_map<Entity, Bolt, EntityHasher> _entityToBolt;
	Graphics& _graphics;
	TransformManager& _transformManager;
	MaterialManager& _materialManager;
	std::default_random_engine _generator;
	const int _randomColorCount = 30;
	DirectX::XMFLOAT3 *_randomColors;
	std::vector<std::tuple<std::uint32_t, std::uint32_t, TextureProxy>> _pregeneratedBuffers;
};

#endif
