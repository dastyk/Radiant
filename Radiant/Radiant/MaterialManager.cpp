#include "MaterialManager.h"

MaterialManager::MaterialManager()
{

}

MaterialManager::~MaterialManager()
{
	
}

void MaterialManager::CreateMaterial(Entity entity, const std::string& shaderName)
{
	std::unordered_map<std::string, SizeAndMapping>::const_iterator got = _shaderNameToSizeAndMapping.find(shaderName);
	//Shader already known
	if (got != _shaderNameToSizeAndMapping.end())
	{

	}
	
}

void MaterialManager::SetFloat(Entity entity, const std::string & materialProperty, float value, uint32_t subMesh)
{
	

}
