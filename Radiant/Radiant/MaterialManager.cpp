#include "MaterialManager.h"
#include "System.h"

MaterialManager::MaterialManager()
{

}

MaterialManager::~MaterialManager()
{
	auto g = System::GetGraphics();
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
			}
		}
		toDelete[j.second.ConstantsMemory] = j.second.ConstantsMemory;
		toDelete[j.second.Textures] = j.second.Textures;
	}

	for (auto &i : _entityToShaderData)
	{
		toDelete[i.second.ConstantsMemory] = i.second.ConstantsMemory;
		toDelete[i.second.Textures] = i.second.Textures;
	}

	for (auto &i : _entityToSubMeshMap)
	{
		for (auto &j : i.second)
		{
			toDelete[j.second.ConstantsMemory] = j.second.ConstantsMemory;
			toDelete[j.second.Textures] = j.second.Textures;
		}
	}

	for (auto &d : toDelete)
	{
		SAFE_DELETE_ARRAY( d.second);
	}
	
	for (auto &t : _textureNameToTexture)
		g->ReleaseTexture(t.second);

//	g->ClearPrimeLine(0);

}

void MaterialManager::BindMaterial(Entity entity, const std::string& shaderName)
{
	auto check = _entityToShaderData.find(entity);
	if (check != _entityToShaderData.end())
	{
		//Oh boy, the entity already has a material. Clean up without messing something else up
		std::unordered_map<void*, void*> toDelete;
		
		toDelete[_entityToShaderData[entity].ConstantsMemory] = _entityToShaderData[entity].ConstantsMemory;
		toDelete[_entityToShaderData[entity].Textures] = _entityToShaderData[entity].Textures;
		for (auto &j : _entityToSubMeshMap[entity])
		{
			toDelete[j.second.ConstantsMemory] = j.second.ConstantsMemory;
			toDelete[j.second.Textures] = j.second.Textures;
		}

		for (auto &d : toDelete)
		{
			SAFE_DELETE_ARRAY(d.second);
		}
	}
	_CreateMaterial(shaderName);
	ShaderData& ref = _shaderNameToShaderData[shaderName];
	ShaderData data = ref;
	data.ConstantsMemory = new char[ref.ConstantsMemorySize];
	memcpy(data.ConstantsMemory, ref.ConstantsMemory, ref.ConstantsMemorySize);
	data.Textures = new TextureProxy[ref.TextureCount];
	memcpy(data.Textures, ref.Textures, sizeof(TextureProxy) * ref.TextureCount);
	_entityToShaderData[entity] = data;

	MaterialCreated( entity, &_shaderNameToShaderData[shaderName] );
}

void MaterialManager::ReleaseMaterial(Entity entity)
{
	std::unordered_map<void*, void*> toDelete;
	auto esd = _entityToShaderData.find(entity);
	if (esd == _entityToShaderData.end())
	{
		return;
	}
	toDelete[esd->second.ConstantsMemory] = esd->second.ConstantsMemory;
	toDelete[esd->second.Textures] = esd->second.Textures;
	for (auto &i : _entityToSubMeshMap[entity])
	{
		toDelete[i.second.ConstantsMemory] = i.second.ConstantsMemory;
		toDelete[i.second.Textures] = i.second.Textures;
	}
	for (auto &d : toDelete)
	{
		SAFE_DELETE_ARRAY(d.second);
	}
	_entityToShaderData.erase(entity);
	_entityToSubMeshMap[entity].clear();
	_entityToSubMeshMap.erase(entity);
}

ShaderData MaterialManager::_CreateMaterial(const std::string& shaderName)
{
	std::unordered_map<std::string, ShaderData>::const_iterator got = _shaderNameToShaderData.find(shaderName);
	
	if (got != _shaderNameToShaderData.end())
	{
		return got->second;
	}
	std::wstring sname = std::wstring(shaderName.begin(), shaderName.end());
	ShaderData data = System::GetGraphics()->GenerateMaterial(sname.c_str());
	_shaderNameToShaderData[shaderName] = data;
	return data;
}

void MaterialManager::SetMaterialProperty(Entity entity, uint32_t subMesh, const std::string& propertyName, float value, const std::string& shaderName)
{
	auto exists = _entityToShaderData.find(entity);
	if (exists == _entityToShaderData.end())
	{
		return;
	}

	ShaderData& data = _entityToShaderData[entity];
	auto c = data.Constants.find(propertyName);
	if (c == data.Constants.end())
	{
		TraceDebug("Warning: Tried to set a nonexistant material property \"%s\" in MaterialManger", propertyName.c_str());
		return;
	}

	std::unordered_map<uint32_t, ShaderData>& subMeshMap = _entityToSubMeshMap[entity];
	//Does entry already exist?
	auto test = subMeshMap.find(subMesh);
	if(test == subMeshMap.end())
	{
		//It didn't exist
		ShaderData d = _CreateMaterial(shaderName); //returns shallow copy of _shadeNameToShaderData[shadername]
		ShaderData::Constant& c = d.Constants[propertyName];
		subMeshMap[subMesh] = d;

		subMeshMap[subMesh].ConstantsMemory = new char[d.ConstantsMemorySize];
		memcpy(subMeshMap[subMesh].ConstantsMemory, d.ConstantsMemory, d.ConstantsMemorySize);//Copy over defaults
		memcpy((char*)subMeshMap[subMesh].ConstantsMemory + c.Offset, &value, c.Size); //Insert new value

		subMeshMap[subMesh].Textures = new TextureProxy[d.TextureCount];
		if (d.Shader != _entityToShaderData[entity].Shader)
			memcpy(subMeshMap[subMesh].Textures, d.Textures, d.TextureCount * sizeof(TextureProxy));
		else
			memcpy(subMeshMap[subMesh].Textures, _entityToShaderData[entity].Textures, subMeshMap[subMesh].TextureCount * sizeof(TextureProxy));

		MaterialChanged( entity, &subMeshMap[subMesh], subMesh );
		
		return;
	}
	else
	{
		//It did exist and we might need to clean up.
		//Reason being, we might have set a new shader to it
		ShaderData& dontDelete = _entityToShaderData[entity];
		ShaderData& sm = subMeshMap[subMesh];
		ShaderData templ = _CreateMaterial(shaderName);
		ShaderData::Constant con = templ.Constants[propertyName];

		//If it uses a different shader all together, its possible none of its previous values were valid
		//Its previous shaderdata might have been pointing to the same stuff as _entityToShaderData[entity]
		//Make sure we dont delete that if thats the case
		if (sm.Shader != templ.Shader)
		{
			if (sm.ConstantsMemory != dontDelete.ConstantsMemory)
			{
				SAFE_DELETE_ARRAY(sm.ConstantsMemory);
				sm.ConstantsMemory = new char[templ.ConstantsMemorySize];
				memcpy(sm.ConstantsMemory, templ.ConstantsMemory, templ.ConstantsMemorySize);
			}
			if (sm.Textures != dontDelete.Textures)
			{
				SAFE_DELETE_ARRAY(sm.Textures);
				sm.Textures = new TextureProxy[templ.TextureCount];
				memcpy(sm.Textures, templ.Textures, templ.TextureCount * sizeof(TextureProxy));
			}
		}
		
		//Copy over the new value to the right place
		memcpy((char*)sm.ConstantsMemory + con.Offset, &value, con.Size);

		MaterialChanged( entity, &sm, subMesh );
		
		return;
	}
}

//this is for the entire entity
void MaterialManager::SetMaterialProperty(Entity entity, const std::string & propertyName, float value, const std::string & shaderName)
{
	ShaderData sd = _CreateMaterial(shaderName);
	auto c = sd.Constants.find(propertyName);
	if (c == sd.Constants.end())
	{
		TraceDebug("Warning: Tried to set a nonexistant material property \"%s\" in MaterialManger", propertyName.c_str());
		return;
	}
	
	auto exists = _entityToShaderData.find(entity);
	if (exists == _entityToShaderData.end())
	{
		return;
	}

	ShaderData& data = exists->second;
	

	//Delete all old materials
	std::unordered_map<void*, void*> toDelete;
	if (sd.Shader != data.Shader)
	{
		toDelete[data.ConstantsMemory] = data.ConstantsMemory;
		toDelete[data.Textures] = data.Textures;
		data = sd;
		data.ConstantsMemory = new char[data.ConstantsMemorySize];
		memcpy(data.ConstantsMemory, sd.ConstantsMemory, data.ConstantsMemorySize);
		memcpy((char*)data.ConstantsMemory + data.Constants[propertyName].Offset, &value, data.Constants[propertyName].Size);

		data.Textures = new TextureProxy[data.TextureCount];
		memcpy(data.Textures, sd.Textures, sizeof(TextureProxy) * sd.TextureCount);
	}
	else
	{
		memcpy((char*)data.ConstantsMemory + data.Constants[propertyName].Offset, &value, sizeof(value));
	}
	for (auto &i : _entityToSubMeshMap[entity])
	{
		toDelete[i.second.ConstantsMemory] = i.second.ConstantsMemory;
		toDelete[i.second.Textures] = i.second.Textures;
	}
	for (auto &d : toDelete)
	{
		SAFE_DELETE_ARRAY(d.second);
	}
	_entityToSubMeshMap[entity].clear();

	MaterialChanged( entity, &data, -1 );
	if (_materialChangeCallbackDecal)
		_materialChangeCallbackDecal(entity, &data);

}

void MaterialManager::SetMaterialProperty(Entity entity, const std::string & propertyName, DirectX::XMFLOAT3 value, const std::string & shaderName)
{
	ShaderData sd = _CreateMaterial(shaderName);
	auto c = sd.Constants.find(propertyName);
	if (c == sd.Constants.end())
	{
		TraceDebug("Warning: Tried to set a nonexistant material property \"%s\" in MaterialManger", propertyName.c_str());
		return;
	}

	auto exists = _entityToShaderData.find(entity);
	if (exists == _entityToShaderData.end())
	{
		return;
	}

	ShaderData& data = exists->second;


	//Delete all old materials
	std::unordered_map<void*, void*> toDelete;
	if (sd.Shader != data.Shader)
	{
		toDelete[data.ConstantsMemory] = data.ConstantsMemory;
		toDelete[data.Textures] = data.Textures;
		data = sd;
		data.ConstantsMemory = new char[data.ConstantsMemorySize];
		memcpy(data.ConstantsMemory, sd.ConstantsMemory, data.ConstantsMemorySize);
		memcpy((char*)data.ConstantsMemory + data.Constants[propertyName].Offset, &value, data.Constants[propertyName].Size);

		data.Textures = new TextureProxy[data.TextureCount];
		memcpy(data.Textures, sd.Textures, sizeof(TextureProxy) * sd.TextureCount);
	}
	else
	{
		memcpy((char*)data.ConstantsMemory + data.Constants[propertyName].Offset, &value, sizeof(value));
	}
	for (auto &i : _entityToSubMeshMap[entity])
	{
		toDelete[i.second.ConstantsMemory] = i.second.ConstantsMemory;
		toDelete[i.second.Textures] = i.second.Textures;
	}
	for (auto &d : toDelete)
	{
		SAFE_DELETE_ARRAY(d.second);
	}
	_entityToSubMeshMap[entity].clear();

	MaterialChanged(entity, &data, -1);
	if (_materialChangeCallbackDecal)
		_materialChangeCallbackDecal(entity, &data);
}

void MaterialManager::SetEntityTexture(Entity entity, const std::vector<std::string>& materialProperties, const std::vector<std::wstring>& textures)
{
	auto f = _entityToShaderData.find(entity);
	if (f == _entityToShaderData.end())
	{
		return;
	}
	if (!(materialProperties.size() > 0))
		return;
	if (!(textures.size() > 0))
		return;

	ShaderData& sd = f->second;

	for (auto& mp : materialProperties)
	{
		auto k = sd.TextureOffsets.find(mp);
		if (k == sd.TextureOffsets.end())
		{
			TraceDebug("Property %s not found", mp.c_str());
			return;
		}
	}
	auto g = System::GetGraphics();

	// If we reached here, the property was found.
	std::vector<TextureProxy> ids(sd.TextureCount);
	for (uint32_t i = 0; i < textures.size(); i++)
	{
		uint32_t offset = sd.TextureOffsets[materialProperties[i]];

		auto got = _textureNameToTexture.find(textures[i]);
		if (got == _textureNameToTexture.end())
			_textureNameToTexture[textures[i]] = g->CreateTexture(textures[i].c_str());
		
		sd.Textures[offset] = _textureNameToTexture[textures[i]];
		ids[offset] = sd.Textures[offset];
	}
	
	sd.TextureWrapp = g->CreateTextureWrapper(ids);

	MaterialChanged(entity, &sd, -1);
	if (_materialChangeCallbackDecal)
		_materialChangeCallbackDecal(entity, &sd);
}


void MaterialManager::SetEntityTexture( Entity entity, const string& materialProperty, const wstring& texture)
{
	auto f = _entityToShaderData.find(entity);
	if (f == _entityToShaderData.end())
	{
		return;
	}

	ShaderData& sd = f->second;
	
	auto k = sd.TextureOffsets.find( materialProperty );
	if ( k == sd.TextureOffsets.end() )
	{
		TraceDebug( "Property %s not found", materialProperty.c_str() );
		return;
	}
	auto g = System::GetGraphics();

	// If we reached here, the property was found.
	uint32_t offset = k->second;
	
	auto got = _textureNameToTexture.find( texture );
	if ( got == _textureNameToTexture.end() )
		_textureNameToTexture[texture] = g->CreateTexture( texture.c_str() );
	
	sd.Textures[offset] = _textureNameToTexture[texture];
	
	std::vector<TextureProxy> ids(sd.TextureCount);
	for (auto& of : sd.TextureOffsets)
	{
		ids[of.second] = sd.Textures[of.second];
	}


	sd.TextureWrapp = g->CreateTextureWrapper(ids);


	MaterialChanged( entity, &sd, -1 );
	if (_materialChangeCallbackDecal)
		_materialChangeCallbackDecal(entity, &sd);
}

void MaterialManager::SetEntityTexture( Entity entity, const string& materialProperty, const TextureProxy& texture )
{
	auto f = _entityToShaderData.find( entity );
	if ( f == _entityToShaderData.end() )
	{
		TraceDebug( "MaterialManager::SetTexture failed, entity not bound in MaterialManager.\n" );
		return;
	}

	ShaderData& sd = f->second;

	auto k = sd.TextureOffsets.find( materialProperty );
	if ( k == sd.TextureOffsets.end() )
	{
		TraceDebug( "Property %s not found", materialProperty.c_str() );
		return;
	}
	auto g = System::GetGraphics();
	// If we reached here, the property was found.
	uint32_t offset = k->second;

	sd.Textures[offset] = texture;

	std::vector<TextureProxy> ids(sd.TextureCount);
	for (auto& of : sd.TextureOffsets)
	{
		ids[of.second] = (sd.Textures[of.second]);
	}


	sd.TextureWrapp = g->CreateTextureWrapper(ids);


	if (_materialChangeCallbackDecal)
		_materialChangeCallbackDecal(entity, &sd);
}

void MaterialManager::SetSubMeshTexture(Entity entity, const std::string & materialProperty, const std::wstring & texture, std::uint32_t subMesh)
{
	auto f = _entityToShaderData.find(entity);
	if (f == _entityToShaderData.end())
	{
		return;
	}
	
	auto got = _textureNameToTexture.find(texture);
	if (got == _textureNameToTexture.end())
		_textureNameToTexture[texture] = System::GetGraphics()->CreateTexture(texture.c_str());

	auto g = _entityToSubMeshMap[entity].find(subMesh);
	if (g == _entityToSubMeshMap[entity].end())
	{
		//Submesh doesnt currently exist
		//Since no material for the submesh is set we assume it wants the same as the entity it belongs to
		_entityToSubMeshMap[entity][subMesh] = f->second;
		ShaderData& sm = _entityToSubMeshMap[entity][subMesh];
		//We need to allocate new memory for it
		sm.Textures = new TextureProxy[sm.TextureCount];
		memcpy(sm.Textures, f->second.Textures, f->second.TextureCount * sizeof(TextureProxy));//Copy over defaults

		auto k = sm.TextureOffsets.find( materialProperty );
		if ( k == sm.TextureOffsets.end() )
		{
			TraceDebug( "Property %s not found", materialProperty.c_str() );
			return;
		}

		uint32_t offset = k->second;
		sm.Textures[offset] = _textureNameToTexture[texture]; //Set current
		
		MaterialChanged( entity, &sm, subMesh );
		
		return;
	}
	else
	{
		//Submesh does exist, we need to check if it points to the default or not
		ShaderData& dontOverwrite = _entityToShaderData[entity];
		ShaderData& current = g->second;

		auto k = current.TextureOffsets.find( materialProperty );
		if ( k == current.TextureOffsets.end() )
		{
			TraceDebug( "Property %s not found", materialProperty.c_str() );
			return;
		}

		uint32_t offset = k->second;

		if (current.Textures == dontOverwrite.Textures)
		{
			current.Textures = new TextureProxy[current.TextureCount];
			//Copy over the defaults
			memcpy(current.Textures, dontOverwrite.Textures, dontOverwrite.TextureCount * sizeof(TextureProxy));
		}
		//Put in the textureID in the right place
		current.Textures[offset] = _textureNameToTexture[texture];

		MaterialChanged( entity, &current, subMesh );
		
		return;
	}
}

float MaterialManager::GetMaterialPropertyOfSubMesh(Entity entity, const std::string & materialProperty, uint32_t subMesh)
{
	auto got = _entityToSubMeshMap[entity].find(subMesh);
	if (got == _entityToSubMeshMap[entity].end())
	{
		return 0.0f;
	}

	float retValue;
	ShaderData::Constant& c = got->second.Constants[materialProperty];
	memcpy(&retValue, (char*)got->second.ConstantsMemory + c.Offset, sizeof(float));
	return retValue;
}

float MaterialManager::GetMaterialPropertyOfEntity(Entity entity, const std::string & materialProperty)
{
	auto got = _entityToShaderData.find(entity);
	if (got == _entityToShaderData.end())
	{
		return 0.0f;
	}
	
	auto got2 = got->second.Constants.find(materialProperty);
	if (got2 == got->second.Constants.end())
	{
		TraceDebug("Tried to set notexistant materia property on entity %d, property \"%s\"", entity.ID, materialProperty.c_str());
		return 0.0f;
	}

	float retValue;
	memcpy(&retValue, (char*)got->second.ConstantsMemory + got2->second.Offset, sizeof(float));
	return retValue;
}

int32_t MaterialManager::GetTextureID(Entity entity, const std::string& texNameInShader)
{
	auto got = _entityToShaderData.find(entity);
	if (got != _entityToShaderData.end())
	{
		ShaderData& d = got->second;
		auto got2 = d.TextureOffsets.find(texNameInShader);
		if (got2 != d.TextureOffsets.end())
		{
			return d.Textures[got2->second].Index;
		}
	}
	return -1;
}
