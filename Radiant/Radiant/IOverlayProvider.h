
#ifndef _IOVERLAY_PROVIDER_H_
#define _IOVERLAY_PROVIDER_H_
#pragma once
#include <functional>
#include <cstdint>
#include <DirectXMath.h>
#include <map>
#include "General.h"
#include <vector>
#include "ShaderData.h"


struct OverlayData
{
	float width;
	float height;
	
	float posX;
	float posY;
	float posZ;

	bool visible;
	bool mat;
	const ShaderData* material;
};


class IOverlayProvider
{
public:
	//virtual void GatherJobs( std::function<const Material*(RenderJob&)> ProvideJob ) = 0;
	virtual void GatherOverlayJobs( std::function<void(OverlayData&)> ProvideJob) = 0;
	//virtual void GatherCam(CamData& Cam) = 0;
};

#endif