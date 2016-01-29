
#ifndef _IOVERLAY_PROVIDER_H_
#define _IOVERLAY_PROVIDER_H_
#pragma once
#include <functional>
#include <cstdint>
#include <DirectXMath.h>
#include <map>
#include "General.h"
#include <vector>



struct OverlayData
{
	uint width;
	uint height;
	
	uint posX;
	uint posY;

	OverlayData(OverlayData&& other)
	{
		width = other.width;
		height = other.height;
		posX = other.posX;
		posY = other.posY;
	}
	OverlayData& operator=(OverlayData&& rhs)
	{
		if (this != &rhs)
		{
			width = rhs.width;
			height = rhs.height;
			posX = rhs.posX;
			posY = rhs.posY;
		}
		return *this;
	}
};


class IOverlayProvider
{
public:
	//virtual void GatherJobs( std::function<const Material*(RenderJob&)> ProvideJob ) = 0;
	virtual void GatherOverlayJobs( std::function<void(OverlayData&)> ProvideJob) = 0;
	//virtual void GatherCam(CamData& Cam) = 0;
};

#endif