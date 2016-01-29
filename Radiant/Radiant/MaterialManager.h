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

	
	void SetMaterialProperty(Entity entity, uint32_t subMesh, const std::string& propertyName, float value, const std::string& shaderName);
	void SetTexture( Entity entity, const std::string& materialProperty, const std::wstring& texture, std::uint32_t subMesh = 0 );

	/*Don't call this function other than in constructor of staticMeshManager*/
	void SetMaterialChangeCallback(std::function<void(Entity, const ShaderData&, uint32_t subMesh)> callback) { _materialChangeCallback = callback; } // submesh
	
	/*Don't call this function other than in constructor of staticMeshManager*/
	void GetSubMeshCount(std::function<int(Entity)> callback) { _GetSubMeshCount = callback; } // submesh
	void SetMaterialChangeCallback2(std::function<void(Entity, const ShaderData&)> callback) { _materialChangeCallback2 = callback; } // overlay


private:
	void _CreateMaterial(const std::string& shaderName);
	std::unordered_map<std::string, ShaderData> _shaderNameToShaderData;
	std::unordered_map<Entity, std::vector<ShaderData>, EntityHasher> _entityToSubMeshMaterial;
	std::unordered_map<Entity, ShaderData, EntityHasher> _entityToShaderData;

	std::unordered_map<std::wstring, std::uint32_t> _textureNameToIndex;

	//Anonymous function notifying staticmeshmanager that a material has been changed
	std::function<void(Entity, const ShaderData&, uint32_t subMesh)> _materialChangeCallback;
	std::function<void(Entity, const ShaderData&)> _materialChangeCallback2;
	//Anonymous function asking staticMeshManager for the submeshcount
	std::function<uint32_t(Entity)> _GetSubMeshCount;
	
};

#endif
