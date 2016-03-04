#include "ShaderData.h"
#include  <DirectXMath.h>

int32_t ShaderData::GenerateSomewhatUniqueID() const
{
	uint32_t somePrimes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71 };

	int32_t retVal = 0;
	//This cant even be called a hash but it will probably work well enough for this and its cheap
	uint32_t texcount = TextureCount;
	uint32_t loopTo = DirectX::XMMin(texcount, 20U); //I mean we won't have more than 20 textures for one material
	for (uint32_t i = 0; i < TextureCount; ++i)
	{
		retVal += Textures[i].Index * somePrimes[i];
	}
	return retVal;
}