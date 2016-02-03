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
	DirectX::XMFLOAT3 DirectionVS;
	float CosOuter;
	float CosInner;
	DirectX::XMFLOAT3 Color;
	DirectX::XMFLOAT3 PositionVS;
	float RangeRcp;
	float Intensity;
};

struct CapsuleLight
{
	DirectX::XMFLOAT3 PositionVS;
	float RangeRcp;
	DirectX::XMFLOAT3 DirectionVS;
	float Length;
	DirectX::XMFLOAT3 Color;
	float Intensity;
};

typedef std::vector<PointLight> PointLightVector;
typedef std::vector<SpotLight> SpotLightVector;
typedef std::vector<CapsuleLight> CapsuleLightVector;

struct DirectionalLight
{
	
};

class ILightProvider
{
public:
	virtual void GatherLights(PointLightVector& pointLights, SpotLightVector& spotLights, CapsuleLightVector& capsuleLights) = 0;
};

#endif
