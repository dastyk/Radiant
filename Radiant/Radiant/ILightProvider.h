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
	
};

struct SpotLight
{

};

struct DirectionalLight
{
	
};

class ILightProvider
{
	virtual void GatherLights(PointLight& pointLights, SpotLight& spotLights, DirectionalLight& directionalLights) = 0;
};

#endif
