#include "DecalManager.h"
#include "System.h"
using namespace DirectX;

DecalManager::DecalManager(MaterialManager& matman, TransformManager& traman) : _materialManager(matman), _transformManager(traman)
{
	matman.SetMaterialChangeCallbackDecal([this](Entity entity, ShaderData* shaderData) {
		MaterialChanged(entity, shaderData);

	});
	traman.TransformChanged += Delegate<void(const Entity&, const XMMATRIX&, const XMVECTOR&, const XMVECTOR&, const XMVECTOR&)>::Make<DecalManager, &DecalManager::_TransformChanged>(this);
	System::GetGraphics()->ClearDecalProviders();
	System::GetGraphics()->AddDecalProvider(this);
}

DecalManager::~DecalManager()
{
	System::GetGraphics()->ClearDecalProviders();
}

void DecalManager::BindDecal(Entity entity)
{
	_entityToDecal[entity] = Decal();
	//The following could be moved to the builder I suppose.
	_materialManager.BindMaterial(entity, "Shaders/DecalsPS.hlsl");
	_materialManager.SetEntityTexture(entity, "gColor", L"Assets/Textures/default_color.png");
	_materialManager.SetEntityTexture(entity, "gNormal", L"Assets/Textures/default_normal.png");
	_materialManager.SetEntityTexture(entity, "gEmissive", L"Assets/Textures/default_displacement.png");
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

void DecalManager::GatherDecals(DecalVector & decals)
{
	for (auto &i : _entityToDecal)
	{
		//Will have to create the world matrix here until I implement
		//a callback from transform to decalmanager
		//XMVECTOR scale = _transformManager.GetScale(i.first);
		//XMFLOAT3 frot;
		//XMVECTOR rotation = _transformManager.GetRotation(i.first);
		//XMStoreFloat3(&frot, rotation);
		//XMVECTOR translation = _transformManager.GetPosition(i.first);
		//XMMATRIX world = XMMatrixScalingFromVector(scale);
		//world = world * XMMatrixRotationX(frot.x) * XMMatrixRotationY(frot.y) * XMMatrixRotationZ(frot.z);
		//world = world * XMMatrixTranslationFromVector(translation);
		//XMStoreFloat4x4(&i.second.World, world);

		decals.push_back(&i.second);
	}
}

void DecalManager::MaterialChanged(Entity entity, ShaderData * shaderData)
{
	auto got = _entityToDecal.find(entity);
	if (got != _entityToDecal.end())
	{
		got->second.shaderData = shaderData;
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
