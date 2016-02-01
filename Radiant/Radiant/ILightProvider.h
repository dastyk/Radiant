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

typedef std::vector<PointLight> PointLightVector;

struct SpotLight
{

};

struct DirectionalLight
{
	
};

class ILightProvider
{
public:
	virtual void GatherLights(PointLightVector& pointLights) = 0;
};

#endif
