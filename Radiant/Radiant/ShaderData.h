#ifndef _SHADER_DATA_H_
#define _SHADER_DATA_H_

#include <cstdint>
#include <unordered_map>

struct ShaderData
{
	struct Constant
	{
		std::uint32_t Offset;
		std::uint32_t Size;
	};

	std::int32_t Shader = -1;
	void *ConstantsMemory = nullptr;
	std::uint32_t ConstantsMemorySize = 0;
	std::unordered_map<std::string, Constant> Constants;
	std::int32_t *Textures = nullptr;
	std::uint32_t TextureCount = 0;
	std::unordered_map<std::string, std::uint32_t> TextureOffsets;

	//This is used by the DecalManager to group decals with the same material before
	//sending it to the renderer
	int32_t GenerateSomewhatUniqueID() const;

};

#endif