#include "LightManager.h"
#include "System.h"

LightManager::LightManager(TransformManager& transformManager) : _graphics(*System::GetGraphics())
{
	_graphics.AddLightProvider(this);

	transformManager.SetTransformChangeCallback5([this]
		(const Entity& entity,
			const DirectX::XMVECTOR& pos,
			const DirectX::XMVECTOR& rot)
	{
		_TransformChanged(entity, pos, rot);
	});
}

LightManager::~LightManager()
{

}

void LightManager::BindPointLight(Entity entity, const DirectX::XMFLOAT3& pos, float range,
	const DirectX::XMFLOAT3& color, float intensity)
{
	_entityToPointLight[entity] = PointLight(pos, range, color, intensity);
}

void LightManager::BindSpotLight(Entity entity, const DirectX::XMFLOAT3 & color, float intensity, float angle)
{
	_entityToSpotLight[entity] = SpotLight(XMFLOAT3(0.0f,0.0f,0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), color, intensity, angle);
}

void LightManager::ChangeLightRange(Entity entity, float range)
{
	auto got = _entityToPointLight.find(entity);

	if (got != _entityToPointLight.end())
	{
		got->second.range = range;
	}
}

void LightManager::ChangeLightIntensity(Entity entity, float intensity)
{
	auto got = _entityToPointLight.find(entity);

	if (got != _entityToPointLight.end())
	{
		got->second.intensity = intensity;
	}
	auto got2 = _entityToSpotLight.find(entity);

	if (got2 != _entityToSpotLight.end())
	{
		got2->second.intensity = intensity;
	}
}

void LightManager::ChangeLightColor(Entity entity, const DirectX::XMFLOAT3& color)
{
	auto got = _entityToPointLight.find(entity);

	if (got != _entityToPointLight.end())
	{
		got->second.color = color;
	}
	auto got2 = _entityToSpotLight.find(entity);

	if (got2 != _entityToSpotLight.end())
	{
		got2->second.color = color;
	}
}

void LightManager::_TransformChanged(const Entity& entity, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& rot)
{
	auto got = _entityToPointLight.find(entity);

	if (got != _entityToPointLight.end())
	{
		DirectX::XMStoreFloat3(&got->second.position, pos);
	}
	auto got2 = _entityToSpotLight.find(entity);

	if (got2 != _entityToSpotLight.end())
	{
		DirectX::XMStoreFloat3(&got2->second.position, pos);
		DirectX::XMStoreFloat3(&got2->second.dir, rot);
	}
}

void LightManager::GatherLights(PointLightVector& pointLights, SpotLightVector& spotlights)
{
	for (auto &plights : _entityToPointLight)
	{
		pointLights.push_back(plights.second);
	}
	for (auto& slight : _entityToSpotLight)
	{
		spotlights.push_back(slight.second);
	}
}

void LightManager::RemovePointLight(Entity entity)
{
	auto i = _entityToPointLight.find(entity);
	if (i == _entityToPointLight.end())
	{
		TraceDebug("Tried to remove non-existant pointlight from entity %d", entity.ID);
		return;
	}
	_entityToPointLight.erase(entity);

}

void LightManager::RemoveSpotLight(Entity entity)
{
	auto i = _entityToSpotLight.find(entity);
	if (i == _entityToSpotLight.end())
	{
		TraceDebug("Tried to remove non-existant spotlight from entity %d", entity.ID);
		return;
	}
	_entityToSpotLight.erase(entity);

}

const void LightManager::BindToRenderer(bool exclusive)
{
	if (exclusive)
		System::GetGraphics()->ClearLightProviders();
	System::GetGraphics()->AddLightProvider(this);
}

