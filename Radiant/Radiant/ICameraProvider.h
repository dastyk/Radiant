#ifndef _ICAMERA_PROVIDER_H_
#define _ICAMERA_PROVIDER_H_


#pragma once
#include <functional>
#include <cstdint>
#include <DirectXMath.h>
#include <map>
#include "General.h"
#include <vector>


struct CameraMatrixes
{
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT4X4 viewProjectionMatrix;
};


class ICameraProvider
{
public:
	//virtual void GatherJobs( std::function<const Material*(RenderJob&)> ProvideJob ) = 0;
	virtual const void GatherCameras( std::function<void(CameraMatrixes&)> ProvideCamera ) = 0;
};

#endif