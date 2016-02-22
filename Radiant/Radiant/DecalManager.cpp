#include "DecalManager.h"
#include "System.h"
using namespace DirectX;

DecalManager::DecalManager(MaterialManager& matman, TransformManager& traman) : _materialManager(matman), _transformManager(traman)
{
	matman.SetMaterialChangeCallbackDecal([this](Entity entity, ShaderData* shaderData) {
		MaterialChanged(entity, shaderData);

	});
	System::GetGraphics()->ClearDecalProviders();
	System::GetGraphics()->AddDecalProvider(this);
}

DecalManager::~DecalManager()
{
	
}

void DecalManager::BindDecal(Entity entity)
{
	_entityToDecal[entity] = Decal();
	_materialManager.BindMaterial(entity, "Shaders/DecalsPS.hlsl");
	_materialManager.SetEntityTexture(entity, "gColor", L"Assets/Textures/default_color.png");
	_materialManager.SetEntityTexture(entity, "gNormal", L"Assets/Textures/default_normal.png");
	_materialManager.SetEntityTexture(entity, "gEmissive", L"Assets/Textures/default_displacement.png");
	_materialManager.SetMaterialProperty(entity, "Roughness", 0.7f, "Shaders/DecalsPS.hlsl");
	_materialManager.SetMaterialProperty(entity, "Metallic", 0.1f, "Shaders/DecalsPS.hlsl");
	
	_transformManager.CreateTransform(entity);
}

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
	}
	
	
}

void DecalManager::GatherDecals(DecalVector & decals)
{
	for (auto &i : _entityToDecal)
	{
		//Will have to create the world matrix here until I implement
		//a callback from transform to decalmanager
		XMVECTOR scale = _transformManager.GetScale(i.first);
		XMFLOAT3 frot;
		XMVECTOR rotation = _transformManager.GetRotation(i.first);
		XMStoreFloat3(&frot, rotation);
		XMVECTOR translation = _transformManager.GetPosition(i.first);
		XMMATRIX world = XMMatrixScalingFromVector(scale);
		world = world * XMMatrixRotationX(frot.x) * XMMatrixRotationY(frot.y) * XMMatrixRotationZ(frot.z);
		world = world * XMMatrixTranslationFromVector(translation);
		XMStoreFloat4x4(&i.second.World, world);

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
