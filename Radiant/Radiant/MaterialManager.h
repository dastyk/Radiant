#ifndef _MATERIAL_MANAGER_H_
#define _MATERIAL_MANAGER_H_
#pragma once

#include <DirectXMath.h>
#include "Entity.h"
#include "Graphics.h"
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
	Graphics::ShaderData GetShaderData(Entity entity, uint32_t subMesh = 0);

	
	void SetMaterialChangeCallback(std::function<void(Entity, const Graphics::ShaderData&, uint32_t subMesh)> callback) { _materialChangeCallback = callback; } // submesh

private:
	
	std::unordered_map<std::string, Graphics::ShaderData> _shaderNameToShaderData;
	std::unordered_map<Entity, std::vector<Graphics::ShaderData>, EntityHasher> _entityToSubMeshMaterial;
	std::unordered_map<Entity, std::string, EntityHasher> _entityToShaderName;

	std::function<void(Entity, const Graphics::ShaderData&, uint32_t subMesh)> _materialChangeCallback; //Submesh, takes precedence over entity material
	
};

#endif
