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
	void GatherLights(PointLightVector& pointLights);

	const void BindToRenderer(bool exclusive);

private:
	void _TransformChanged(const Entity& entity, const DirectX::XMVECTOR& pos, const DirectX::XMMATRIX& rotation);
	std::unordered_map<Entity, PointLight, EntityHasher> _entityToPointLight;

private:
	Graphics& _graphics;
};

#endif
