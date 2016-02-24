#ifndef _LIGHTNING_H_
#define _LIGHTNING_H_

#include "IEffectProvider.h"

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
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

private:
	struct Segment
	{
		DirectX::XMFLOAT3 Start;
		DirectX::XMFLOAT3 End;
	};

	struct Bolt
	{
		DirectX::XMFLOAT3 Base;
		DirectX::XMFLOAT3 Target;
		std::uint32_t VertexBuffer;
		const ShaderData *Material;
		std::vector<Segment> Segments;
	};

private:
	void _TransformChanged( const Entity& entity, const DirectX::XMMATRIX& transform, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& up );
	void _MaterialChanged( Entity entity, const ShaderData* material );
	void _SetDefaultMaterials( const Entity& entity, const ShaderData* material );

private:
	std::vector<Bolt> _bolts;
	std::unordered_map<Entity, unsigned, EntityHasher> _entityToIndex;
	Graphics& _graphics;
	ShaderData _shader;
};

#endif