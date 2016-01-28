#ifndef _MATERIAL_MANAGER_H_
#define _MATERIAL_MANAGER_H_
#pragma once

#include <DirectXMath.h>
#include "Entity.h"
#include "FileHandler.h"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <map>

class MaterialManager
{
public:
	MaterialManager();
	~MaterialManager();

	void CreateMaterial(Entity entity, const std::string& shaderName);
	void SetFloat(Entity entity, const std::string& materialProperty, float value);



private:
	struct MapAndMemory
	{
		std::unordered_map<std::string, uint32_t> _nameToIndex;
		float* _memory;
	};
	std::vector<std::string> _processedShaders;
	std::unordered_map<Entity, std::vector<MapAndMemory>> _entityToMaterial;
	
};

#endif
