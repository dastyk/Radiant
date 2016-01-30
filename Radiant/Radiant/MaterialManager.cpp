#include "MaterialManager.h"
#include "System.h"

MaterialManager::MaterialManager()
{

}

MaterialManager::~MaterialManager()
{
	//Make sure we don't try to delete the same address twice :^)
	std::unordered_map<void*, void*> toDelete;
	for (auto &j : _shaderNameToShaderData)
	{
		for (auto &i : _entityToSubMeshMaterial)
		{
			for (auto &k : i.second)
			{
				toDelete[k.ConstantsMemory] = k.ConstantsMemory;
				toDelete[k.Textures] = k.Textures;
				//SAFE_DELETE_ARRAY( k.Textures );
			}
		}
		toDelete[j.second.ConstantsMemory] = j.second.ConstantsMemory;
		toDelete[j.second.Textures] = j.second.Textures;
		//SAFE_DELETE_ARRAY( j.second.Textures );
	}

	for (auto &i : _entityToShaderData)
	{
		toDelete[i.second.ConstantsMemory] = i.second.ConstantsMemory;
		toDelete[i.second.Textures] = i.second.Textures;
	}

	for (auto &d : toDelete)
	{
		SAFE_DELETE_ARRAY( d.second);
	}
	
}

void MaterialManager::BindMaterial(Entity entity, const std::string& shaderName)
{
	_CreateMaterial(shaderName);
	ShaderData& ref = _shaderNameToShaderData[shaderName];
	ShaderData data = ref;
	data.ConstantsMemory = new char[ref.ConstantsMemorySize];
	memcpy(data.ConstantsMemory, ref.ConstantsMemory, ref.ConstantsMemorySize);
	data.Textures = new int32_t[ref.TextureCount];
	memcpy(data.Textures, ref.Textures, sizeof(int32_t) * ref.TextureCount);
	_entityToShaderData[entity] = data;
	_materialCreatedCallback(entity, _shaderNameToShaderData[shaderName]);
}

void MaterialManager::_CreateMaterial(const std::string& shaderName)
{
	std::unordered_map<std::string, ShaderData>::const_iterator got = _shaderNameToShaderData.find(shaderName);
	
	if (got != _shaderNameToShaderData.end())
		return;
	std::wstring sname = std::wstring(shaderName.begin(), shaderName.end());
	ShaderData data = System::GetGraphics()->GenerateMaterial(sname.c_str());
	_shaderNameToShaderData[shaderName] = data;
}

void MaterialManager::SetMaterialProperty(Entity entity, uint32_t subMesh, const std::string& propertyName, float value, const std::string& shaderName)
{
	auto exists = _entityToShaderData.find(entity);
	if (exists == _entityToShaderData.end())
	{
		TraceDebug("Warning: Tried to set material of unbound entity %d in MaterialManger", entity.ID);
		return;
	}

	ShaderData& data = _shaderNameToShaderData[shaderName];
	auto c = data.Constants.find(propertyName);
	if (c == data.Constants.end())
	{
		TraceDebug("Warning: Tried to set a nonexistant material property \"%s\" in MaterialManger", propertyName.c_str());
		return;
	}

	//TODO: Perhaps it would be better to save a map of entity->submeshCount and check it
	//before calling the staticmeshmanager to do it. Probably not much of a difference in 
	//performance though.
	uint32_t subMeshCount = _GetSubMeshCount(entity);
	
	//If 0 is returned it means it doesn't have a mesh on it (yet)
	if (subMeshCount == 0)
	{
		TraceDebug("Tried to set material of submesh %d of entity %d but no mesh is bound.\n", subMesh, entity.ID);
		return;
	}

	if (subMesh >= subMeshCount)
	{
		throw(ErrorMsg(1100001U, L"Index of submesh exceeds submeshcount.\n"));
	}
	
	
	std::vector<ShaderData>& subMeshes = _entityToSubMeshMaterial[entity];
	//Check if there's already an entry
	if (subMeshes.size() == 0)
		{
		//Assume other submeshes of this entity will use the same shader unless otherwise specified.
		subMeshes.resize(subMeshCount, data);
	}
	
	subMeshes[subMesh] = data; //Default values for shader

	//Check if current shaderdata is pointing to the same ConstantsMemory as the template
	if (subMeshes[subMesh].ConstantsMemory == data.ConstantsMemory)
	{
		//If it is, we must allocate new memory
		subMeshes[subMesh].ConstantsMemory = new char[data.ConstantsMemorySize];
	}
	
	//Replace old value with new one.
	memcpy((char*)subMeshes[subMesh].ConstantsMemory + c->second.Offset, &value, c->second.Size);


	//TODO: Find a better way of doing this stuff
	if(_materialChangeCallback)
		_materialChangeCallback(entity, subMeshes[subMesh], subMesh);
	if (_materialChangeCallback2)
		_materialChangeCallback2(entity, subMeshes[subMesh]);

}


void MaterialManager::SetTexture( Entity entity, const string& materialProperty, const wstring& texture, uint32_t subMesh )
{
	auto f = _entityToShaderData.find(entity);
	if (f == _entityToShaderData.end())
	{
		TraceDebug("MaterialManager::SetTexture failed, entity not bound in MaterialManager.\n");
		return;
	}
	std::vector<ShaderData>& subMeshes = _entityToSubMeshMaterial[entity];
	
	ShaderData sd;
	if (subMesh < subMeshes.size())
	{
		//If submesh exists, modify that material instead
		sd = subMeshes[subMesh];
	}
	else
	{
		sd = _entityToShaderData[entity];
	}
	
	uint32_t offset = sd.TextureOffsets[materialProperty];
	
	int32_t textureID = -1;
	auto got = _textureNameToIndex.find( texture );
	if ( got != _textureNameToIndex.end() )
		textureID = got->second;
	else
		textureID = System::GetGraphics()->CreateTexture( texture.c_str() );
	
	sd.Textures[offset] = textureID;

	if (_materialChangeCallback)
		_materialChangeCallback( entity, sd, subMesh );
	if (_materialChangeCallback2)
		_materialChangeCallback2(entity, sd);
}


