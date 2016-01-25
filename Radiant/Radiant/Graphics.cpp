#include "Graphics.h"
#include "System.h"


Graphics::Graphics()
{
	_device = nullptr;
	_deviceContext = nullptr;
	_swapChain = nullptr;
	_depthStencilState = nullptr;
	_blendState = nullptr;
	_rasterState = nullptr;

	 _renderTarget = nullptr;
	_depthStencil = nullptr;

#if defined(DEBUG) || defined(_DEBUG)

	_debugger = nullptr;

#endif
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
	if (_depthStencilState)
	{
		_depthStencilState->Release();
		_depthStencilState = nullptr;
	}
	if (_blendState)
	{
		_blendState->Release();
		_blendState = nullptr;
	}
	if (_rasterState)
	{
		_rasterState->Release();
		_rasterState = nullptr;
	}
	if (_renderTarget)
	{
		_renderTarget->Release();
		delete _renderTarget;
		_renderTarget = nullptr;
	}
	if (_depthStencil)
	{
		_depthStencil->Release();
		delete _depthStencil;
		_depthStencil = nullptr;
	}

#if defined(DEBUG) || defined(_DEBUG)
	if (_debugger)
	{
		_debugger->Release();
		_debugger = nullptr;
	}
#endif
	return void();
}

const void Graphics::SetRenderTargetToBackBuffer() const
{
	if (!_deviceContext)
		throw ErrorMsg(5000015, L"No device context found.");
	_deviceContext->OMSetRenderTargets(1, &_renderTarget->renderTargetView, _depthStencil->depthStencilView);
	return void();
}

const void Graphics::_InitDirect3D()
{
	UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create the devices and swapchain
	_CreateDeviceAndDeviceContextAndSwapChain(createDeviceFlags);


#if defined(DEBUG) || defined(_DEBUG)
	HRESULT hr = _device->QueryInterface(__uuidof(ID3D11Debug), (void**)&_debugger);
	if (FAILED(hr))
	{
		throw ErrorMsg(5000006, L"Failed to get debugger.");
	}
#endif

	_CreateRenderTargetFromBackBuffer();
	_CreateDepthStencilBufferView();
	_CreateBlendState();
	_CreateRasterState();

	WindowHandler* w = System::GetInstance()->GetWindowHandler();
	_SetViewPort((float)w->GetWindowWidth(), (float)w->GetWindowHeight());

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
		nullptr,
		flags,
		nullptr,
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


	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;


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
		nullptr,
		flags,
		nullptr,
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

const void Graphics::_CreateRenderTargetFromBackBuffer(D3D11_RENDER_TARGET_VIEW_DESC * pRTVDesc)
{
	HRESULT hr;

	// Create render target view
	try { _renderTarget = new RenderTarget(); }
	catch (exception& e)
	{
		throw ErrorMsg(5000008, L"Failed to create rendertarget.");
	}

	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&_renderTarget->renderTargetTextureArray);
	if (FAILED(hr))
	{
		throw ErrorMsg(5000005, L"Failed to get backbuffer");
	}

	hr = _device->CreateRenderTargetView(_renderTarget->renderTargetTextureArray, pRTVDesc, &_renderTarget->renderTargetView);
	if (FAILED(hr))
	{
		throw ErrorMsg(5000007, L"Failed to create render target view from backbuffer.");
	}

	_renderTarget->renderTargetTextureArray->Release();
	_renderTarget->renderTargetTextureArray = nullptr;

	return void();
}

const void Graphics::_CreateDepthStencilBufferView()
{
	System* s = System::GetInstance();
	WindowHandler* w = s->GetWindowHandler();
	HRESULT hr;

	// Create depth/stencil buffer and view
	D3D11_TEXTURE2D_DESC depthBufferDesc;

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = w->GetWindowWidth();
	depthBufferDesc.Height = w->GetWindowHeight();
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	if (false)//mEnable4xMsaa)
	{
		depthBufferDesc.SampleDesc.Count = 4;
		depthBufferDesc.SampleDesc.Quality = 0;// m4xMsaaQuality - 1;

	}
	else
	{
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
	}
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	try { _depthStencil = new DepthStencil(); }
	catch (exception& e)
	{
		throw ErrorMsg(5000009, L"Failed to create depthstencil.");
	}

	hr = _device->CreateTexture2D(&depthBufferDesc, 0, &_depthStencil->depthStencilBuffer);
	if (FAILED(hr))
	{
		throw ErrorMsg(5000010, L"Failed to create depthstencilbuffer.");
	}


	hr = _device->CreateDepthStencilView(_depthStencil->depthStencilBuffer, nullptr, &_depthStencil->depthStencilView);
	if (FAILED(hr))
	{
		throw ErrorMsg(5000011, L"Failed to create depthstencilview.");
	}



	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;


	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;// D3D11_COMPARISON_GREATER_EQUAL;// ;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	hr = _device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilState);
	if (FAILED(hr))
	{
		throw ErrorMsg(5000012, L"Failed to create depthstencilstate.");
	}

	// Set the depth stencil state.
	_deviceContext->OMSetDepthStencilState(_depthStencilState, 1);



	//D3D11_RECT rect;
	//rect.top = 0;
	//rect.left = 0;
	//rect.right = mClientWidth;
	//rect.bottom = mClientHeight;

	//mDeviceContext->RSSetScissorRects(1, &rect);

	return void();
}

const void Graphics::_CreateBlendState()
{
	HRESULT hr;
	D3D11_BLEND_DESC blendStateDescription;
	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = false;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	hr = _device->CreateBlendState(&blendStateDescription, &_blendState);
	if (FAILED(hr))
	{
		throw ErrorMsg(5000013, L"Failed to create blendstate.");
	}
	float color[4] = { 0.0f };
	_deviceContext->OMSetBlendState(_blendState, color, 0xffffffff);


	return void();
}

const void Graphics::_CreateRasterState()
{
	HRESULT hr;

	D3D11_RASTERIZER_DESC rasterDesc;
	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = true;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	hr = _device->CreateRasterizerState(&rasterDesc, &_rasterState);
	if (FAILED(hr))
	{
		throw ErrorMsg(5000014, L"Failed to create rasterizerstate.");
	}

	// Now set the rasterizer state.
	_deviceContext->RSSetState(_rasterState);
	return void();
}

const void Graphics::_SetViewPort(float width, float height)
{

	// Set viewport
	_windowViewPort.Width = width;
	_windowViewPort.Height = height;
	_windowViewPort.MinDepth = 0.0f;
	_windowViewPort.MaxDepth = 1.0f;
	_windowViewPort.TopLeftX = 0;
	_windowViewPort.TopLeftY = 0;

	_deviceContext->RSSetViewports(1, &_windowViewPort);


	return void();
}
