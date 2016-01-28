
#ifndef _ICAMERA_PROVIDER_H_
#define _ICAMERA_PROVIDER_H_
#pragma once
#include <functional>
#include <cstdint>
#include <DirectXMath.h>
#include <map>
#include "General.h"
#include <vector>



struct CamData
{
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT4X4 viewProjectionMatrix;
};


class ICameraProvider
{
public:
	//virtual void GatherJobs( std::function<const Material*(RenderJob&)> ProvideJob ) = 0;
	//virtual void GatherCam( std::function<void(CamData&)> ProvideCam ) = 0;
	virtual void GatherCam(CamData& Cam) = 0;
};

#endif