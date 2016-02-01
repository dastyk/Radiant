#include "LightManager.h"
#include "System.h"

LightManager::LightManager(TransformManager& transformManager) : _graphics(*System::GetGraphics())
{
	_graphics.AddLightProvider(this);

	transformManager.SetTransformChangeCallback4([this]
		(const Entity& entity,
			const DirectX::XMVECTOR& pos,
			const DirectX::XMMATRIX& rotation)
	{
		_TransformChanged(entity, pos, rotation);
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

void LightManager::_TransformChanged(const Entity& entity, const DirectX::XMVECTOR& pos, const DirectX::XMMATRIX& rotation)
{
	//Rotation is currently unused as we haven't implemented spotligths yet
	auto got = _entityToPointLight.find(entity);

	if (got != _entityToPointLight.end())
	{
		DirectX::XMStoreFloat3(&got->second.position, pos);
	}
}

void LightManager::GatherLights(PointLightVector& pointLights)
{
	for (auto &plights : _entityToPointLight)
	{
		pointLights.push_back(plights.second);
	}
}

