
#ifndef _ICAMERA_PROVIDER_H_
#define _ICAMERA_PROVIDER_H_
#pragma once
#include <functional>
#include <cstdint>
#include <DirectXMath.h>
#include <map>
#include "General.h"
#include <vector>



struct CameraData
{
	float fov;
	float aspect;
	float nearp;
	float viewDistance;
	float farp;

	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT4X4 viewProjectionMatrix;
	DirectX::XMFLOAT3 camPos;


	CameraData() : fov(90.0f), aspect(1.0f), nearp(0.1f), farp(100.0f), viewDistance(100.0f), viewMatrix(DirectX::XMFLOAT4X4()), projectionMatrix(DirectX::XMFLOAT4X4()), viewProjectionMatrix(DirectX::XMFLOAT4X4())
	{
	}


};


class ICameraProvider
{
public:
	//virtual void GatherJobs( std::function<const Material*(RenderJob&)> ProvideJob ) = 0;
	//virtual void GatherCam( std::function<void(CamData&)> ProvideCam ) = 0;
	virtual void GatherCam(CameraData*& Cam) = 0;
};

#endif