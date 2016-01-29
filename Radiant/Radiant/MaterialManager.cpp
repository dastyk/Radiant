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
			}
		}
		toDelete[j.second.ConstantsMemory] = j.second.ConstantsMemory;
	}

	for (auto &d : toDelete)
	{
		delete[] d.second;
	}
	
}

void MaterialManager::CreateMaterial(Entity entity, const std::string& shaderName)
{
	std::unordered_map<std::string, Graphics::ShaderData>::const_iterator got = _shaderNameToShaderData.find(shaderName);
	_entityToShaderName[entity] = shaderName;
	
	if (got != _shaderNameToShaderData.end())
	{
		//Shader already known
		return;
	}
	//Previously unused shader
	std::wstring sname = std::wstring(shaderName.begin(), shaderName.end());
	Graphics::ShaderData data = System::GetGraphics()->GenerateMaterial(sname.c_str());
	_shaderNameToShaderData[shaderName] = data;
}

void MaterialManager::SetFloat(Entity entity, const std::string & materialProperty, float value, uint32_t subMesh)
{
	std::vector<Graphics::ShaderData>& subMeshes = _entityToSubMeshMaterial[entity];
	Graphics::ShaderData& sd = _shaderNameToShaderData[_entityToShaderName[entity]];
	Graphics::ShaderData::Constant& c = sd.Constants[materialProperty];
	//Index for this submesh already exists in materialvector
	if (subMeshes.size() > subMesh)
	{
		//Some submeshes might point to the default
		//Make sure we don't copy over the default
		if (subMeshes[subMesh].ConstantsMemory != sd.ConstantsMemory)
		{
			memcpy((char*)subMeshes[subMesh].ConstantsMemory + c.Offset, &value, c.Size);
		}
		else
		{
			
			subMeshes[subMesh].ConstantsMemory = new char[sd.ConstantsMemorySize];
			//First copy over default values
			memcpy(subMeshes[subMesh].ConstantsMemory, sd.ConstantsMemory, sd.ConstantsMemorySize);
			//Set specific value
			memcpy((char*)subMeshes[subMesh].ConstantsMemory + c.Offset, &value, c.Size);
		}
		return;
	}
	else
	{
		if (subMeshes.size() <= subMesh)
		{
			subMeshes.resize(subMesh + 1, sd); //Initialize to point at default material
		}
		subMeshes[subMesh].ConstantsMemory = new char[sd.ConstantsMemorySize];
		memcpy(subMeshes[subMesh].ConstantsMemory, sd.ConstantsMemory, sd.ConstantsMemorySize);
		memcpy((char*)subMeshes[subMesh].ConstantsMemory + c.Offset, &value, c.Size);
	}
}

Graphics::ShaderData MaterialManager::GetShaderData(Entity entity, uint32_t subMesh)
{
	Graphics::ShaderData data = _shaderNameToShaderData[_entityToShaderName[entity]];
	//Default material if no submesh materials are defined
	if (_entityToSubMeshMaterial[entity].size() <= subMesh)
	{
		return data;
	}
	data.ConstantsMemory = _entityToSubMeshMaterial[entity][subMesh].ConstantsMemory;
	return data;
}
