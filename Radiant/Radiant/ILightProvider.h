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
	PointLight(const DirectX::XMFLOAT3& p, float r, const DirectX::XMFLOAT3& c, float i, bool v): position(p), range(r), color(c), intensity(i), volumetrick(v)
	{
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
	PointLight operator=(const PointLight& other)
	{
		if (this == &other)
			return *this;
		position = other.position;
		range = other.range;
		color = other.color;
		intensity = other.intensity;
		return *this;
	}
	DirectX::XMFLOAT3 position;
	float range;
	DirectX::XMFLOAT3 color;
	float intensity;
	DirectX::XMFLOAT3 pad = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
	bool volumetrick = false;

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

struct AreaRectLight
{
	AreaRectLight()
	{
		memset(this, 0, sizeof(AreaRectLight));
	}
	AreaRectLight(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, float range, const DirectX::XMFLOAT3& right,
		float rightExtent, float upExtent, const DirectX::XMFLOAT3& color, float intensity)
	{
		Position = position;
		Normal = normal;
		Range = range;
		Right = right;
		RightExtent = rightExtent;
		UpExtent = upExtent;
		Color = color;
		Intensity = intensity;
	}

	AreaRectLight(const AreaRectLight& other)
	{
		Position = other.Position;
		Normal = other.Normal;
		Range = other.Range;
		Right = other.Right;
		RightExtent = other.RightExtent;
		UpExtent = other.UpExtent;
		Color = other.Color;
		Intensity = other.Intensity;
	}
	AreaRectLight operator=(const AreaRectLight& other)
	{
		if (this == &other)
			return *this;
		Position = other.Position;
		Normal = other.Normal;
		Range = other.Range;
		Right = other.Right;
		RightExtent = other.RightExtent;
		UpExtent = other.UpExtent;
		Color = other.Color;
		Intensity = other.Intensity;
		return *this;
	}
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	float Range;
	DirectX::XMFLOAT3  Right;
	float RightExtent;
	float UpExtent;
	DirectX::XMFLOAT3 Color;
	float Intensity;
};


typedef std::vector<PointLight*> PointLightVector;// TODO: Change these to pointers
typedef std::vector<SpotLight*> SpotLightVector;
typedef std::vector<CapsuleLight*> CapsuleLightVector;
typedef std::vector<AreaRectLight*> AreaRectLightVector;

struct DirectionalLight
{
	
};

class ILightProvider
{
public:
	virtual void GatherLights(PointLightVector& pointLights, SpotLightVector& spotLights, CapsuleLightVector& capsuleLights, AreaRectLightVector& areaRectLights) = 0;
};

#endif
