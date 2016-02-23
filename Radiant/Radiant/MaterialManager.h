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

	void BindMaterial(Entity entity, const std::string& shaderName);
	void ReleaseMaterial(Entity entity);
	void SetMaterialProperty(Entity entity, uint32_t subMesh, const std::string& propertyName, float value, const std::string& shaderName);
	void SetMaterialProperty(Entity entity, const std::string& propertyName, float value, const std::string& shaderName);
	void SetEntityTexture( Entity entity, const std::string& materialProperty, const std::wstring& texture);
	void SetSubMeshTexture(Entity entity, const std::string& materialProperty, const std::wstring& texture, std::uint32_t subMesh = 0);
	float GetMaterialPropertyOfSubMesh(Entity entity, const std::string& materialProperty, uint32_t subMesh);
	float GetMaterialPropertyOfEntity(Entity entity, const std::string& materialProperty);

	int32_t GetTextureID(Entity entity,const std::string& texNameInShader);

	/*Don't call this function other than in constructor of staticMeshManager*/
	void SetMaterialChangeCallback(std::function<void(Entity, ShaderData*, uint32_t subMesh)> callback) { _materialChangeCallback = callback; } // submesh
	void SetMaterialCreatedCallback(std::function<void(Entity, ShaderData*)> callback) { _materialCreatedCallback = callback; }
	void SetMaterialEntireEntityCallback(std::function<void(Entity, ShaderData*)> callback){ _materialChangedEntireEntityCallback = callback; }//entire entity
	
	
	void SetMaterialChangeCallback2(std::function<void(Entity, ShaderData*)> callback) { _materialChangeCallback2 = callback; } // overlay

	void SetMaterialChangeCallbackDecal(std::function<void(Entity, ShaderData*)> callback) { _materialChangeCallbackDecal = callback; }


private:
	ShaderData _CreateMaterial(const std::string& shaderName);

	//Provides a template for material, never used by an actual entity
	std::unordered_map<std::string, ShaderData> _shaderNameToShaderData;

	//Unique for all entities, no shared pointers
	std::unordered_map<Entity, ShaderData, EntityHasher> _entityToShaderData;

	//Maps an entity to an array of ShaderData, some are a shallow copy of entityToShaderData
	std::unordered_map<Entity, std::vector<ShaderData>, EntityHasher> _entityToSubMeshMaterial;

	//Lets try this
	std::unordered_map<Entity, std::unordered_map<uint32_t, ShaderData>, EntityHasher> _entityToSubMeshMap;

	std::unordered_map<std::wstring, std::uint32_t> _textureNameToIndex;

	//Anonymous function notifying staticmeshmanager that a material has been changed
	std::function<void(Entity, ShaderData*)> _materialCreatedCallback;
	std::function<void(Entity, ShaderData*, uint32_t subMesh)> _materialChangeCallback;
	std::function<void(Entity, ShaderData*)> _materialChangedEntireEntityCallback;
	std::function<void(Entity, ShaderData*)> _materialChangeCallback2;
	std::function<void(Entity, ShaderData*)> _materialChangeCallbackDecal;
	
	
};

#endif
