#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

//////////////
// Includes //
//////////////
#include <d3d11.h>

////////////////////
// Local Includes //
////////////////////
#include "General.h"
#include "Direct3D11.h"

#pragma comment (lib, "d3d11.lib")

class Graphics
{
public:
	Graphics();
	~Graphics();

	void Start( HWND hWnd, std::uint32_t width, std::uint32_t height );
	const void Init();
	const void Shutdown();

	void Render( double totalTime, double deltaTime );

private:
	const void _InitDirect3D();

	const void _CreateDeviceAndDeviceContext(uint flags, D3D_DRIVER_TYPE pDriverType = D3D_DRIVER_TYPE_HARDWARE);
	const void _CreateDeviceAndDeviceContextAndSwapChain(uint flags, D3D_DRIVER_TYPE pDriverType = D3D_DRIVER_TYPE_HARDWARE);

	const void _CreateRenderTargetView(D3D11_RENDER_TARGET_VIEW_DESC* pRTVDesc);

	HRESULT OnCreateDevice( void );
	void OnDestroyDevice( void );
	HRESULT OnResizedSwapChain( void );
	void OnReleasingSwapChain( void );

	void BeginFrame( void );
	void EndFrame( void );

private:
	Direct3D11 *_D3D11 = nullptr;
	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;
	IDXGISwapChain* _swapChain;
	ID3D11RenderTargetView *_backBufferRTV = nullptr;
	std::uint32_t _width;
	std::uint32_t _height;
};

#endif