#ifndef _ILIGHT_PROVIDER_H_
#define _ILIGHT_PROVIDER_H_
#pragma once

#include <functional>
#include <cstdint>
#include <DirectXMath.h>
#include <map>
#include "General.h"
#include <vector>

struct PointLight
{
	PointLight(const DirectX::XMFLOAT3& p, float r, const DirectX::XMFLOAT3& c, float i)
	{
		position = p;
		range = r;
		color = c;
		intensity = i;
	}
	PointLight()
	{
		memset(this, 0, sizeof(PointLight));
	}
	PointLight(const PointLight& other)
	{
		position = other.position;
		range = other.range;
		color = other.color;
		intensity = other.intensity;
	}
	DirectX::XMFLOAT3 position;
	float range;
	DirectX::XMFLOAT3 color;
	float intensity;
};


struct SpotLight
{
	SpotLight(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT3& d, const DirectX::XMFLOAT3& c, float i, float a)
	{
		position = p;
		dir = d;
		color = c;
		intensity = i;
		angle = a;
	}
	SpotLight()
	{
		memset(this, 0, sizeof(PointLight));
	}
	SpotLight(const SpotLight& other)
	{
		position = other.position;
		dir = other.dir;
		color = other.color;
		intensity = other.intensity;
		angle = other.angle;
	}
	DirectX::XMFLOAT3 position;
	float intensity;
	DirectX::XMFLOAT3 dir;
	float angle;
	DirectX::XMFLOAT3 color;
};

typedef std::vector<PointLight> PointLightVector;
typedef std::vector<SpotLight> SpotLightVector;
struct DirectionalLight
{
	
};

class ILightProvider
{
public:
	virtual void GatherLights(PointLightVector& pointLights, SpotLightVector& spotlights) = 0;
};

#endif
