#ifndef _IEFFECT_PROVIDER_H_
#define _IEFFECT_PROVIDER_H_

#include <vector>
#include <cstdint>
#include "ShaderData.h"
#include "Entity.h"

struct Effect
{
	std::uint32_t VertexBuffer;
	std::uint32_t VertexCount;
	const ShaderData* Material = nullptr;
};

class IEffectProvider
{
public:
	virtual void GatherEffects( std::vector<Effect>& effects ) = 0;
};

#endif