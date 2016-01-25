#include "Graphics.h"

#include <d3dcompiler.h>
#include <DirectXMath.h> // for now
#include "System.h"

using namespace std;
using namespace DirectX;

Graphics::Graphics() : _width( 0 ), _height( 0 )
{
	_D3D11 = new Direct3D11();
	_device = nullptr;
	_deviceContext = nullptr;
	_swapChain = nullptr;
}


Graphics::~Graphics()
{
	_D3D11->Shutdown();
	delete _D3D11;
}

void Graphics::Render( double totalTime, double deltaTime )
{
	BeginFrame();


	EndFrame();
}

HRESULT Graphics::OnCreateDevice( void )
{
	return S_OK;
}


void Graphics::OnDestroyDevice( void )
{

}


HRESULT Graphics::OnResizedSwapChain( void )
{
	return S_OK;
}


void Graphics::OnReleasingSwapChain( void )
{
	//_D3D11->DeleteDepthBuffer( _mainDepth );
}

void Graphics::Start( HWND hWnd, uint32_t width, uint32_t height )
{
	if (!_D3D11->Start( hWnd, width, height ))
		throw "Failed to initialize Direct3D11";

	_device = _D3D11->GetDevice();
	_deviceContext = _D3D11->GetDeviceContext();
	//_FeatureLevel = _D3D11->mFeatureLevel;
	_swapChain = _D3D11->GetSwapChain();
	//_DXGIOutput = _D3D11->mDXGIOutput;
	_backBufferRTV = _D3D11->GetBackBufferRTV();

	_width = width;
	_height = height;

	if (FAILED( OnCreateDevice() ))
		throw "Failed to create device";
	if (FAILED( OnResizedSwapChain() ))
		throw "Failed to resize swap chain";
}

void Graphics::BeginFrame( void )
{
	if (!_deviceContext)
		return;

	static float clearColor[4] = { 0.0f, 0.0f, 0.25f, 1.0f };

	_deviceContext->ClearRenderTargetView( _backBufferRTV, clearColor );
}

void Graphics::EndFrame( void )
{
	//_swapChain->Present( _vSync ? 1 : 0, 0 );
	_swapChain->Present( 1, 0 );
}

const void Graphics::Init()
{
	_InitDirect3D();

	return void();
}

const void Graphics::Shutdown()
{
	//if (_swapChain)
	//{
	//	_swapChain->SetFullscreenState(false, nullptr);
	//	_swapChain->Release();
	//	_swapChain = nullptr;
	//}
	//if (_deviceContext)
	//{
	//	_deviceContext->Release();
	//	_deviceContext = nullptr;
	//}
	//if (_device)
	//{
	//	_device->Release();
	//	_device = nullptr;
	//}

	OnReleasingSwapChain();
	OnDestroyDevice();

	return void();
}

const void Graphics::_InitDirect3D()
{
	UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	_CreateDeviceAndDeviceContextAndSwapChain(createDeviceFlags);
	return void();
}


const void Graphics::_CreateDeviceAndDeviceContext(uint flags, D3D_DRIVER_TYPE pDriverType)
{
	HRESULT hr;
	D3D_FEATURE_LEVEL featureLevel;

	// Create Device and Context
	hr = D3D11CreateDevice(
		nullptr,
		pDriverType,
		NULL,
		flags,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&_device,
		&featureLevel,
		&_deviceContext);

	if (FAILED(hr))
	{
		throw ErrorMsg(5000003, L"Failed to create device, devicecontext.");
	}
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		throw ErrorMsg(5000002, L"Direct3D Feature Level 11 is unsupported.");
	}
	return void();
}

const void Graphics::_CreateDeviceAndDeviceContextAndSwapChain(uint flags, D3D_DRIVER_TYPE pDriverType)
{
	System* s = System::GetInstance();
	WindowHandler* w = s->GetWindowHandler();
	HRESULT hr;
	D3D_FEATURE_LEVEL featureLevel;

	DXGI_SWAP_CHAIN_DESC sd;
	// clear out the struct for use
	ZeroMemory(&sd, sizeof(sd));

	sd.BufferDesc.Width = w->GetWindowWidth();
	sd.BufferDesc.Height = w->GetWindowHeight();
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (true)//mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = 0;//m4xMsaaQuality - 1;

	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	sd.BufferCount = 1;
	sd.OutputWindow = w->GetHWnd();
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Create Device and Context and SwapChain
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		pDriverType,
		NULL,
		flags,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&_swapChain,
		&_device,
		&featureLevel,
		&_deviceContext);
	if (FAILED(hr))
	{
		throw ErrorMsg(5000001, L"Failed to create device, devicecontext and swapchain.");
	}
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		throw ErrorMsg(5000002, L"Direct3D Feature Level 11 is unsupported.");
	}
	return void();
}
