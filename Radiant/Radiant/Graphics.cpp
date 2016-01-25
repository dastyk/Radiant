#include "Graphics.h"
#include "System.h"


Graphics::Graphics()
{
	_device = nullptr;
	_deviceContext = nullptr;
	_swapChain = nullptr;
}


Graphics::~Graphics()
{
}

const void Graphics::Init()
{
	_InitDirect3D();

	return void();
}

const void Graphics::Shutdown()
{
	if (_swapChain)
	{
		_swapChain->SetFullscreenState(false, nullptr);
		_swapChain->Release();
		_swapChain = nullptr;
	}
	if (_deviceContext)
	{
		_deviceContext->Release();
		_deviceContext = nullptr;
	}
	if (_device)
	{
		_device->Release();
		_device = nullptr;
	}

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


const void Graphics::_CreateDeviceAndDeviceContext(uint flags, D3D_DRIVER_TYPE pDriverType = D3D_DRIVER_TYPE_HARDWARE)
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

const void Graphics::_CreateDeviceAndDeviceContextAndSwapChain(uint flags, D3D_DRIVER_TYPE pDriverType = D3D_DRIVER_TYPE_HARDWARE)
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
