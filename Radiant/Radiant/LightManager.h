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
	void BindSpotLight(Entity entity,const DirectX::XMFLOAT3& color, float intensity, float outerAngle, float innerAngle, float range);
	void BindCapsuleLight();
	void BindAreaRectLight(Entity entity, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, float range, const DirectX::XMFLOAT3& right,
		float rightExtent, float upExtent, const DirectX::XMFLOAT3& color, float intensity);
	void GatherLights(PointLightVector& pointLights, SpotLightVector& spotLights, CapsuleLightVector& capsuleLights, AreaRectLightVector& areaLights);
	void RemovePointLight(Entity entity);
	void RemoveSpotLight(Entity entity);
	void RemoveCapsuleLight( Entity entity );
	void RemoveAreaRectLight(Entity entity);

	const void SetAsVolumetric(const Entity& entity, bool vol);
	const void BindToRenderer(bool exclusive);

	const void ToggleVisible(const Entity& entity, int visible);
	void SetInFrustum(const Entity& entity, bool infrustum);
	void SetInFrustum(std::vector<Entity>& entites);

	void ChangeLightRange(const Entity& entity, float range);
	void ChangeLightIntensity(const Entity& entity, float intensity);
	void ChangeLightColor(const Entity& entity, const DirectX::XMFLOAT3& color);
	void ChangeLightBlobRange(const Entity& entity, float range);

	DirectX::XMFLOAT3 GetLightColor(const Entity& entity);
	float GetLightIntensity(const Entity& entity);
	float GetLightRange(const Entity& entity);
	float GetLightBlobRange(const Entity& entity);

private:
	void _TransformChanged( const Entity& entity, const DirectX::XMMATRIX& tran, const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& dir, const DirectX::XMVECTOR& up );

private:
	Graphics& _graphics;
	std::unordered_map<Entity, PointLight, EntityHasher> _entityToPointLight;
	std::unordered_map<Entity, SpotLight, EntityHasher> _entityToSpotLight;
	std::unordered_map<Entity, CapsuleLight, EntityHasher> _entityToCapsuleLight;
	std::unordered_map<Entity, AreaRectLight, EntityHasher> _entityToAreaRectLight;
};

#endif
