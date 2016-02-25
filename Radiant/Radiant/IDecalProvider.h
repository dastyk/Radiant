#ifndef _IDECAL_PROVIDER_H_
#define _IDECAL_PROVIDER_H_

#include <DirectXMath.h>
#include <vector>
#include "ShaderData.h"

struct Decal
{
	const DirectX::XMFLOAT4X4* World;
	const ShaderData* shaderData;
};

struct DecalGroup
{
	uint32_t indexStart;
	uint32_t indexCount;
	
};
typedef std::vector<DecalGroup*> DecalGroupVector;
typedef std::vector<Decal*> DecalVector;

class IDecalProvider
{
public:
	virtual void GatherDecals(DecalVector& d, DecalGroupVector& dgv) = 0;
};

#endif

