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
#include "ShaderData.h"

class MaterialManager
{
public:
	MaterialManager();
	~MaterialManager();

	void CreateMaterial(Entity entity, const std::string& shaderName);
	void SetFloat(Entity entity, const std::string& materialProperty, float value, uint32_t subMesh = 0);
	void SetTexture( Entity entity, const std::string& materialProperty, const std::wstring& texture, std::uint32_t subMesh = 0 );
	ShaderData GetShaderData(Entity entity, uint32_t subMesh = 0);

	
	void SetMaterialChangeCallback(std::function<void(Entity, const ShaderData&, uint32_t subMesh)> callback) { _materialChangeCallback = callback; } // submesh

private:
	
	std::unordered_map<std::string, ShaderData> _shaderNameToShaderData;
	std::unordered_map<Entity, std::vector<ShaderData>, EntityHasher> _entityToSubMeshMaterial;
	std::unordered_map<Entity, std::string, EntityHasher> _entityToShaderName;

	std::unordered_map<std::wstring, std::uint32_t> _textureNameToIndex;

	std::function<void(Entity, const ShaderData&, uint32_t subMesh)> _materialChangeCallback; //Submesh, takes precedence over entity material
	
};

#endif
