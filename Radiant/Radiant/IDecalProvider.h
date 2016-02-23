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

typedef std::vector<Decal*> DecalVector;

class IDecalProvider
{
public:
	virtual void GatherDecals(DecalVector& d) = 0;
};

#endif

