#include "MaterialManager.h"

MaterialManager::MaterialManager()
{

}

MaterialManager::~MaterialManager()
{
	for (auto &i : _entityToShaderData)
	{
		SAFE_DELETE_ARRAY(i.second.ConstantsMemory);
	}
}

void MaterialManager::CreateMaterial(Entity entity, const std::string& shaderName)
{
	std::unordered_map<std::string, Graphics::ShaderData*>::const_iterator got = _shaderNameToShaderData.find(shaderName);
	//Shader already known
	if (got != _shaderNameToShaderData.end())
	{
		Graphics::ShaderData* data = &_entityToShaderData[entity];
		memcpy(data, got->second, sizeof(Graphics::ShaderData));
		data->ConstantsMemory = new char[data->ConstantsMemorySize];
		memcpy(data->ConstantsMemory, got->second, data->ConstantsMemorySize);
		return;
	}
	//Previously unused shader
	std::wstring sname = std::wstring(shaderName.begin(), shaderName.end());
	Graphics::ShaderData data = System::GetGraphics()->GenerateMaterial(sname.c_str());
	memcpy(&_entityToShaderData[entity], &data, sizeof(Graphics::ShaderData));
	_shaderNameToShaderData[shaderName] = &_entityToShaderData[entity];
}

void MaterialManager::SetFloat(Entity entity, const std::string & materialProperty, float value, uint32_t subMesh)
{
	Graphics::ShaderData* data = &_entityToShaderData[entity];
	memcpy(data + data->Constants[materialProperty].Offset, &value, data->Constants[materialProperty].Size);

}
