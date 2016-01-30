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
	data.Textures = new int32_t[ref.TextureCount];
	memcpy(data.Textures, ref.Textures, sizeof(int32_t) * ref.TextureCount);
	_entityToShaderData[entity] = data;
	if(_materialCreatedCallback)
		_materialCreatedCallback(entity, _shaderNameToShaderData[shaderName]);
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
		TraceDebug("Warning: Tried to set material of unbound entity %d in MaterialManger", entity.ID);
		return;
	}

	ShaderData& data = _entityToShaderData[entity];
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
	
	/*TEST AREA*/
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

		subMeshMap[subMesh].Textures = new int32_t[d.TextureCount];
		memcpy(subMeshMap[subMesh].Textures, d.Textures, d.TextureCount * sizeof(int32_t));

		if(_materialChangeCallback)
			_materialChangeCallback(entity, subMeshMap[subMesh], subMesh);
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
				sm.Textures = new int32_t[templ.TextureCount];
				memcpy(sm.Textures, templ.Textures, templ.TextureCount * sizeof(int32_t));
			}
		}
		
		//Copy over the new value to the right place
		memcpy((char*)sm.ConstantsMemory + con.Offset, &value, con.Size);

		if (_materialChangeCallback)
			_materialChangeCallback(entity, sm, subMesh);
		return;
	}

	/*END TEST AREA*/
	
	//std::vector<ShaderData>& subMeshes = _entityToSubMeshMaterial[entity];
	////Check if there's already an entry
	//if (subMeshes.size() == 0)
	//	{
	//	//Assume other submeshes of this entity will use the same shader unless otherwise specified.
	//	subMeshes.resize(subMeshCount, data);
	//}
	//
	//subMeshes[subMesh] = data; //Default values for shader

	////Check if current shaderdata is pointing to the same ConstantsMemory as the template
	//if (subMeshes[subMesh].ConstantsMemory == data.ConstantsMemory)
	//{
	//	//If it is, we must allocate new memory
	//	subMeshes[subMesh].ConstantsMemory = new char[data.ConstantsMemorySize];
	//}
	//
	////Replace old value with new one.
	//memcpy((char*)subMeshes[subMesh].ConstantsMemory + c->second.Offset, &value, c->second.Size);


	////TODO: Find a better way of doing this stuff
	//if(_materialChangeCallback)
	//	_materialChangeCallback(entity, subMeshes[subMesh], subMesh);
	//if (_materialChangeCallback2)
	//	_materialChangeCallback2(entity, subMeshes[subMesh]);

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

void MaterialManager::SetSubMeshTexture(Entity entity, const std::string & materialProperty, const std::wstring & texture, std::uint32_t subMesh)
{
	auto f = _entityToShaderData.find(entity);
	if (f == _entityToShaderData.end())
	{
		TraceDebug("MaterialManager::SetSubMeshTexture failed, entity not bound in MaterialManager.\n");
		return;
	}
	
	int32_t textureID = -1;

	auto got = _textureNameToIndex.find(texture);
	if (got != _textureNameToIndex.end())
	{
		textureID = got->second;
	}
	else
	{
		textureID = System::GetGraphics()->CreateTexture(texture.c_str());
	}

	auto g = _entityToSubMeshMap[entity].find(subMesh);
	if (g == _entityToSubMeshMap[entity].end())
	{
		//Submesh doesnt currently exist
		//Since no material for the submesh is set we assume it wants the same as the entity it belongs to
		_entityToSubMeshMap[entity][subMesh] = f->second;
		ShaderData& sm = _entityToSubMeshMap[entity][subMesh];
		//We need to allocate new memory for it
		sm.Textures = new int32_t[sm.TextureCount];
		memcpy(sm.Textures, f->second.Textures, f->second.TextureCount * sizeof(int32_t));//Copy over defaults
		sm.Textures[sm.TextureOffsets[materialProperty]] = textureID; //Set current
		
		if (_materialChangeCallback)
			_materialChangeCallback(entity, sm, subMesh);
		return;
	}
	else
	{
		//Submesh does exist, we need to check if it points to the default or not
		ShaderData& dontOverwrite = _entityToShaderData[entity];
		ShaderData& current = g->second;
		uint32_t offset = current.TextureOffsets[materialProperty];

		if (current.Textures == dontOverwrite.Textures)
		{
			current.Textures = new int32_t[current.TextureCount];
			//Copy over the defaults
			memcpy(current.Textures, dontOverwrite.Textures, dontOverwrite.TextureCount * sizeof(int32_t));
		}
		//Put in the textureID in the right place
		current.Textures[offset] = textureID;
		if (_materialChangeCallback)
			_materialChangeCallback(entity, current, subMesh);
		return;
	}
}


