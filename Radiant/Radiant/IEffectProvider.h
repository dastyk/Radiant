#ifndef _IEFFECT_PROVIDER_H_
#define _IEFFECT_PROVIDER_H_

#include <vector>
#include <cstdint>
#include <DirectXMath.h>
#include "ShaderData.h"
#include "Entity.h"

struct Effect
{
	std::uint32_t VertexBuffer;
	std::uint32_t VertexCount;
	const ShaderData* Material = nullptr;
	DirectX::XMFLOAT4X4 World;
};

class IEffectProvider
{
public:
	virtual void GatherEffects( std::vector<Effect>& effects ) = 0;
};

#endif