#include "ShaderData.h"

int32_t ShaderData::GenerateSomewhatUniqueID() const
{
	int32_t retVal = 0;
	//This cant even be called a hash but it will probably work well enough for this and its cheap
	for (uint32_t i = 0; i < TextureCount; ++i)
	{
		retVal += Textures[i].Index * i;//The first texture for decals will always be the depth buffer which isnt part of the material
	}
	return retVal;
}