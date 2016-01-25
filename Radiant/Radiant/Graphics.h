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

//Structs
struct RenderTarget
{	
	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D* renderTargetTextureArray;
	ID3D11ShaderResourceView* shaderResourceViewArray;

	RenderTarget()
	{
		renderTargetView = nullptr;
		renderTargetTextureArray = nullptr;
		shaderResourceViewArray = nullptr;
	}

	const void Release()
	{
		if (renderTargetView)
		{
			renderTargetView->Release();
			renderTargetView = nullptr;
		}
		if (renderTargetTextureArray)
		{
			renderTargetTextureArray->Release();
			renderTargetTextureArray = nullptr;
		}
		if (shaderResourceViewArray)
		{
			shaderResourceViewArray->Release();
			shaderResourceViewArray = nullptr;
		}
	}
};

struct DepthStencil
{
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;

	DepthStencil()
	{
		depthStencilView = nullptr;
		depthStencilBuffer = nullptr;
	}

	const void Release()
	{
		if (depthStencilView)
		{
			depthStencilView->Release();
			depthStencilView = nullptr;
		}
		if (depthStencilBuffer)
		{
			depthStencilBuffer->Release();
			depthStencilBuffer = nullptr;
		}
	}
};
class Graphics
{
public:
	Graphics();
	~Graphics();

	const void Init();
	const void Shutdown();

	const void SetRenderTargetToBackBuffer()const;
private:
	const void _InitDirect3D();

	const void _CreateDeviceAndDeviceContext(uint flags, D3D_DRIVER_TYPE pDriverType = D3D_DRIVER_TYPE_HARDWARE);
	const void _CreateDeviceAndDeviceContextAndSwapChain(uint flags, D3D_DRIVER_TYPE pDriverType = D3D_DRIVER_TYPE_HARDWARE);

	const void _CreateRenderTargetFromBackBuffer(D3D11_RENDER_TARGET_VIEW_DESC* pRTVDesc = nullptr);
	const void _CreateDepthStencilBufferView();
	const void _CreateBlendState();
	const void _CreateRasterState();
	const void _SetViewPort(float width, float height);


private:
	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;
	IDXGISwapChain* _swapChain;

	ID3D11DepthStencilState* _depthStencilState;
	ID3D11BlendState* _blendState;
	ID3D11RasterizerState* _rasterState;

	RenderTarget* _renderTarget;
	DepthStencil* _depthStencil;

	D3D11_VIEWPORT _windowViewPort;
	
#if defined(DEBUG) || defined(_DEBUG)

	ID3D11Debug* _debugger;

#endif
};

#endif