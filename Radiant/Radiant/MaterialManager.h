#ifndef _MATERIAL_MANAGER_H_
#define _MATERIAL_MANAGER_H_
#pragma once

#include <DirectXMath.h>
#include "Entity.h"
#include "FileHandler.h"
#include "StaticMeshManager.h"
#include "System.h"
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
	void SetFloat(Entity entity, const std::string& materialProperty, float value, uint32_t subMesh = 0);



private:
	Graphics::ShaderData _data;

	struct SizeAndMapping
	{
		uint32_t _size;
		std::unordered_map<std::string, uint32_t> _nameToindex;
	};
	std::unordered_map<std::string, Graphics::ShaderData*> _shaderNameToShaderData;
	std::unordered_map<Entity, Graphics::ShaderData, EntityHasher> _entityToShaderData;
	
};

#endif
