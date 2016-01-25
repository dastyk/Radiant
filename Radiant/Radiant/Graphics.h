#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#pragma once
//////////////
// Includes //
//////////////
#include <d3d11.h>

////////////////////
// Local Includes //
////////////////////
#include "General.h"


#pragma comment (lib, "d3d11.lib")


class Graphics
{
public:
	Graphics();
	~Graphics();

	const void Init();
	const void Shutdown();


private:
	const void _InitDirect3D();

	const void _CreateDeviceAndDeviceContext(uint flags, D3D_DRIVER_TYPE pDriverType = D3D_DRIVER_TYPE_HARDWARE);
	const void _CreateDeviceAndDeviceContextAndSwapChain(uint flags, D3D_DRIVER_TYPE pDriverType = D3D_DRIVER_TYPE_HARDWARE);

	const void _CreateRenderTargetView(D3D11_RENDER_TARGET_VIEW_DESC* pRTVDesc);
private:
	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;
	IDXGISwapChain* _swapChain;
};

#endif