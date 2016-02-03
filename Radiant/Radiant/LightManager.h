#ifndef _LIGHT_MANAGER_H_
#define _LIGHT_MANAGER_H_
#pragma once

#include "ILightProvider.h"
#include "Entity.h"
#include "TransformManager.h"
#include "Graphics.h"
#include <unordered_map>

class LightManager : public ILightProvider
{
public:
	LightManager(TransformManager& transformManager);
	~LightManager();

	void BindPointLight(Entity entity, const DirectX::XMFLOAT3& pos, float range,
		const DirectX::XMFLOAT3& color, float intensity);
	void BindSpotLight(Entity entity,const DirectX::XMFLOAT3& color, float intensity, float angle);
	void GatherLights(PointLightVector& pointLights, SpotLightVector& spotlights);
	void RemovePointLight(Entity entity);
	void RemoveSpotLight(Entity entity);

	const void BindToRenderer(bool exclusive);

	void ChangeLightRange(Entity entity, float range);
	void ChangeLightIntensity(Entity entity, float intensity);
	void ChangeLightColor(Entity entity, const DirectX::XMFLOAT3& color);
private:
	void _TransformChanged(const Entity& entity, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& rot);
	std::unordered_map<Entity, PointLight, EntityHasher> _entityToPointLight;
	std::unordered_map<Entity, SpotLight, EntityHasher> _entityToSpotLight;

private:
	Graphics& _graphics;
};

#endif
