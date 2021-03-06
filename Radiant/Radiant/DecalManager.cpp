#include "DecalManager.h"
#include "System.h"
using namespace DirectX;

DecalManager::DecalManager(MaterialManager& matman, TransformManager& traman) : _materialManager(matman), _transformManager(traman)
{
	matman.MaterialChanged2 += Delegate<void(const Entity& entity, const ShaderData* data)>::Make<DecalManager, &DecalManager::_MaterialChanged>(this);
	traman.TransformChanged += Delegate<void(const Entity&, const XMMATRIX&, const XMVECTOR&, const XMVECTOR&, const XMVECTOR&)>::Make<DecalManager, &DecalManager::_TransformChanged>(this);
	System::GetGraphics()->ClearDecalProviders();
	System::GetGraphics()->AddDecalProvider(this);
}

DecalManager::~DecalManager()
{
	//Nope cant have this here, put it before adding in constructor instead otherwise it allgets removed
	//System::GetGraphics()->ClearDecalProviders();
}

void DecalManager::BindDecal(Entity entity)
{
	if (_entityToDecal.size() == 256)
	{
		auto iter = _entityToDecal.begin();
		_cachedWorldTransforms.erase(iter->first);
		_entityToDecal.erase(iter);
		
	}
	_entityToDecal[entity] = Decal();
	//The following could be moved to the builder I suppose.
	_materialManager.BindMaterial(entity, "Shaders/DecalsPS.hlsl");

	std::vector<string> pro;
	pro.push_back("gColor");
	pro.push_back("gNormal");
	pro.push_back("gEmissive");

	std::vector<wstring> texs;
	texs.push_back(L"Assets/Textures/default_color.png");
	texs.push_back(L"Assets/Textures/default_normal.png");
	texs.push_back(L"Assets/Textures/default_displacement.png");



	_materialManager.SetEntityTexture(entity, pro, texs);

	_materialManager.SetMaterialProperty(entity, "Roughness", 0.5f, "Shaders/DecalsPS.hlsl");
	_materialManager.SetMaterialProperty(entity, "Metallic", 0.2f, "Shaders/DecalsPS.hlsl");
	
	_transformManager.CreateTransform(entity);
}

//The following functions just interface with the materialmanager, making it simpler
//to set the textures for the decals. i.e. you dont have to know what the shader resource
//view is called in the shader.
void DecalManager::SetColorTexture(Entity entity, const wchar_t * name)
{
	auto got = _entityToDecal.find(entity);
	if(got != _entityToDecal.end())
		_materialManager.SetEntityTexture(entity, "gColor", name);
}

void DecalManager::SetNormalTexture(Entity entity, const wchar_t * name)
{
	auto got = _entityToDecal.find(entity);
	if (got != _entityToDecal.end())
		_materialManager.SetEntityTexture(entity, "gNormal", name);
}

void DecalManager::SetEmissiveTexture(Entity entity, const wchar_t * name)
{
	auto got = _entityToDecal.find(entity);
	if (got != _entityToDecal.end())
		_materialManager.SetEntityTexture(entity, "gEmissive", name);
}

void DecalManager::SetRoughness(Entity entity, float value)
{
	_materialManager.SetMaterialProperty(entity, "Roughness", value, "Shaders/DecalsPS.hlsl");
}

void DecalManager::SetMetallic(Entity entity, float value)
{
	_materialManager.SetMaterialProperty(entity, "Metallic", value, "Shaders/DecalsPS.hlsl");
}

void DecalManager::ReleaseDecal(Entity entity)
{
	auto got = _entityToDecal.find(entity);
	if (got != _entityToDecal.end())
	{
		_materialManager.ReleaseMaterial(entity);
		_entityToDecal.erase(got);
		_cachedWorldTransforms.erase(entity);
	}
	
	
}

void DecalManager::GatherDecals(DecalVector & decals, DecalGroupVector& dgv)
{
	_decalVectors.clear();
	_decalGroups.clear();
	decals.reserve(_entityToDecal.size());
	for (auto &i : _entityToDecal)
	{
		//Perhaps the somewhatuniqueID could be saved in the Decal struct and updated
		//whenever the material changes.
		//_decalVectors[i.second.shaderData->GenerateSomewhatUniqueID()].push_back(&i.second);
		_decalVectors[(uint32_t)i.second.shaderData->TextureWrapp].push_back(&i.second);
	}
	dgv.reserve(_decalVectors.size());
	uint32_t indexStart = 0;
	uint32_t indexCount = 0;
	for (auto &i : _decalVectors)
	{
		
		indexCount = static_cast<uint32_t>(i.second.size());
		_decalGroups[i.first].indexCount = indexCount;
		_decalGroups[i.first].indexStart = indexStart;
		indexStart += indexCount;
	}

	for (auto &i : _decalVectors)
	{
		for (auto &j : i.second)
		{
			decals.push_back(j);
		}
	}
	for (auto &i : _decalGroups)
	{
		dgv.push_back(&i.second);
	}
}

void DecalManager::BindToRenderer(bool yes)
{
	if (yes)
		System::GetGraphics()->ClearDecalProviders();
	System::GetGraphics()->AddDecalProvider(this);
}

void DecalManager::_MaterialChanged(const Entity& entity, const ShaderData* data)
{
	auto got = _entityToDecal.find(entity);
	if (got != _entityToDecal.end())
	{
		got->second.shaderData = data;
	}
}

void DecalManager::_TransformChanged(const Entity & entity, const DirectX::XMMATRIX & transform, const DirectX::XMVECTOR & pos, const DirectX::XMVECTOR & dir, const DirectX::XMVECTOR & up)
{
	auto got = _entityToDecal.find(entity);
	if (got != _entityToDecal.end())
	{
		XMStoreFloat4x4(&_cachedWorldTransforms[entity], transform);
		got->second.World = &_cachedWorldTransforms[entity];
	}
}
