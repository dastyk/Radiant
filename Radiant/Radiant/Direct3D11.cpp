#include "Direct3D11.h"
#include "Utils.h"
#include "General.h"
#include "System.h"

// Initializes Direct3D11 by creating the device, context, and swap chain
// with back buffer.
bool Direct3D11::Start(HWND hWnd, unsigned backbufferWidth, unsigned backbufferHeight)
{
	Options* o = System::GetOptions();
	this->_hWnd = hWnd;

	IDXGIFactory *dxgiFactory = nullptr;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&dxgiFactory)))
	{
		TraceDebug("Couldn't create DXGIFactory");
		return false;
	}

	IDXGIAdapter *dxgiAdapter = nullptr;
	if (FAILED(dxgiFactory->EnumAdapters(0, &dxgiAdapter)))
	{
		TraceDebug("Failed to get adapter");
		return false;
	}

	if (FAILED(dxgiAdapter->EnumOutputs(0, &_DXGIOutput)))
	{
		TraceDebug("Failed to get output");
		return false;
	}

	unsigned deviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
	unsigned numFeatureLevels = sizeof(featureLevels) / sizeof(featureLevels[0]);

	HRESULT hr = D3D11CreateDevice(
		dxgiAdapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		deviceFlags,
		featureLevels,
		numFeatureLevels,
		D3D11_SDK_VERSION,
		&_d3dDevice,
		&_FeatureLevel,
		&_d3dDeviceContext);

	if (FAILED(hr))
	{
		TraceDebug("Failed to create device");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC sd;
	memset(&sd, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	if (o->GetFullscreen())
	{
		backbufferWidth = o->GetScreenResolutionWidth();
		backbufferHeight = o->GetScreenResolutionHeight();
	}
	sd.BufferDesc.Width = backbufferWidth;
	sd.BufferDesc.Height = backbufferHeight;

	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.RefreshRate.Numerator = o->GetRefreshRateNumerator();
	sd.BufferDesc.RefreshRate.Denominator = o->GetRefreshRateDenominator();
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.OutputWindow = hWnd;
	sd.Windowed = true;

	if (FAILED(dxgiFactory->CreateSwapChain(_d3dDevice, &sd, &_SwapChain)))
	{
		TraceDebug("Failed to create swapchain");
		return false;
	}

	// Make sure DXGI does not interfere with window messages.
	// I'll just handle Alt+Enter myself, thank you.
	dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

	SAFE_RELEASE(dxgiAdapter);
	SAFE_RELEASE(dxgiFactory);

	if (!Resize(backbufferWidth, backbufferHeight))
		return false;

	return true;
}

void Direct3D11::Shutdown( void )
{
	// TODO: Release all created resources

	SAFE_RELEASE( _BackBufferRTV );
	SAFE_RELEASE( _SwapChain );
	SAFE_RELEASE( _d3dDeviceContext );

	// Release device and notify if a com object has not been properly released.
	if ( _d3dDevice )
	{
		UINT references = _d3dDevice->Release();
		if ( references > 0 )
		{
			TraceDebug( "A com object has not been released." );
			throw ErrorMsg( 5000016, L"The Direct3D device reference count is not 0, which means at least one object has not been released. GLHF" );
		}
		_d3dDevice = nullptr;
	}

	SAFE_RELEASE( _DXGIOutput );
}

bool Direct3D11::Resize( unsigned width, unsigned height )
{
	if ( _SwapChain )
	{
		// Unbind any buffers
		_d3dDeviceContext->OMSetRenderTargets( 0, nullptr, nullptr );

		// Release old render target view as it references the buffer that
		// will be destroyed.
		if ( _BackBufferRTV ) _BackBufferRTV->Release();

		// Preserve number of existing buffers and existing back buffer format.
		HRESULT hr = _SwapChain->ResizeBuffers( 0, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );

		if ( FAILED( hr ) )
		{
			TraceDebug( "Failed to resize buffers!" );
			return false;
		}

		// Recreate render target view to back buffer.

		ID3D11Texture2D *backBufferTexture = nullptr;
		hr = _SwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture) );

		if ( FAILED( hr ) )
		{
			TraceDebug( "Failed to get the swap chain back buffer!" );
			return false;
		}

		hr = _d3dDevice->CreateRenderTargetView( backBufferTexture, nullptr, &_BackBufferRTV );

		SAFE_RELEASE( backBufferTexture );

		if ( FAILED( hr ) )
		{
			TraceDebug( "Failed to create the render target view!" );
			return false;
		}

		_d3dDeviceContext->OMSetRenderTargets( 1, &_BackBufferRTV, nullptr );

		D3D11_VIEWPORT viewport;
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast <float>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		_d3dDeviceContext->RSSetViewports( 1, &viewport );
	}

	return true;
}

RenderTarget Direct3D11::CreateRenderTarget(
	DXGI_FORMAT format,
	unsigned width,
	unsigned height,
	unsigned depth,
	unsigned flags,
	unsigned arraySize,
	unsigned mipLevels,
	unsigned sampleCount )
{
	RenderTarget renderTarget;

	// Create the underlying resource.

	// 1D
	if ( height == 0 && depth == 0 )
	{
		D3D11_TEXTURE1D_DESC texDesc;
		texDesc.Width = width;
		texDesc.MipLevels = mipLevels;
		texDesc.ArraySize = arraySize;
		texDesc.Format = format;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if ( flags & TEXTURE_COMPUTE_WRITE )
			texDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

		if ( flags & TEXTURE_GEN_MIPS )
			texDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

		HRESULT hr = _d3dDevice->CreateTexture1D( &texDesc, nullptr, (ID3D11Texture1D**)&renderTarget.Texture );
		if ( FAILED( hr ) )
		{
			TraceDebug( "Failed to create 1D render target texture" );
			TraceHR( hr );
			return RenderTarget();
		}

		renderTarget.SliceCount = texDesc.ArraySize;
	}
	// 2D
	else if ( depth == 0 )
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = mipLevels;
		texDesc.ArraySize = arraySize * ((flags & TEXTURE_CUBE) ? 6 : 1);
		texDesc.Format = format;
		texDesc.SampleDesc.Count = sampleCount;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if ( flags & TEXTURE_COMPUTE_WRITE )
			texDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

		if ( flags & TEXTURE_GEN_MIPS )
			texDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

		if ( flags & TEXTURE_CUBE )
			texDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

		HRESULT hr = _d3dDevice->CreateTexture2D( &texDesc, nullptr, (ID3D11Texture2D**)&renderTarget.Texture );
		if ( FAILED( hr ) )
		{
			TraceDebug( "Failed to create 2D render target texture" );
			TraceHR( hr );
			return RenderTarget();
		}

		renderTarget.SliceCount = texDesc.ArraySize;
	}
	// 3D
	else
	{
		D3D11_TEXTURE3D_DESC texDesc;
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.Depth = depth;
		texDesc.MipLevels = mipLevels;
		texDesc.Format = format;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if ( flags & TEXTURE_COMPUTE_WRITE )
			texDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

		if ( flags & TEXTURE_GEN_MIPS )
			texDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

		HRESULT hr = _d3dDevice->CreateTexture3D( &texDesc, nullptr, (ID3D11Texture3D**)&renderTarget.Texture );
		if ( FAILED( hr ) )
		{
			TraceDebug( "Failed to create 3D render target texture" );
			TraceHR( hr );
			return RenderTarget();
		}

		renderTarget.SliceCount = depth;
	}

	// Create views to texture.

	renderTarget.RTV = _CreateRTV( renderTarget.Texture );
	renderTarget.SRV = _CreateSRV( renderTarget.Texture );
	renderTarget.UAV = _CreateUAV( renderTarget.Texture );

	renderTarget.RTVSlices = new ID3D11RenderTargetView *[renderTarget.SliceCount];
	renderTarget.SRVSlices = new ID3D11ShaderResourceView *[renderTarget.SliceCount];
	renderTarget.UAVSlices = new ID3D11UnorderedAccessView *[renderTarget.SliceCount];
	for ( unsigned i = 0; i < renderTarget.SliceCount; ++i )
	{
		renderTarget.RTVSlices[i] = _CreateRTV( renderTarget.Texture, format, i );
		renderTarget.SRVSlices[i] = _CreateSRV( renderTarget.Texture, format, i );
		renderTarget.UAVSlices[i] = _CreateUAV( renderTarget.Texture, format, i );
	}

	// Cache stuff
	renderTarget.Format = format;
	renderTarget.Width = width;
	renderTarget.Height = height;
	renderTarget.Depth = depth;
	renderTarget.ArraySize = arraySize;
	renderTarget.MipLevels = mipLevels;
	renderTarget.SampleCount = sampleCount;

	return renderTarget;
}

void Direct3D11::DeleteRenderTarget( RenderTarget &rt )
{
	SAFE_RELEASE( rt.Texture );
	SAFE_RELEASE( rt.RTV );
	SAFE_RELEASE( rt.SRV );
	SAFE_RELEASE( rt.UAV );

	if ( rt.RTVSlices )
	{
		for ( unsigned i = 0; i < rt.SliceCount; ++i )
		{
			SAFE_RELEASE( rt.RTVSlices[i] );
		}

		SAFE_DELETE_ARRAY( rt.RTVSlices );
	}

	if ( rt.SRVSlices )
	{
		for ( unsigned i = 0; i < rt.SliceCount; ++i )
		{
			SAFE_RELEASE( rt.SRVSlices[i] );
		}

		SAFE_DELETE_ARRAY( rt.SRVSlices );
	}

	if ( rt.UAVSlices )
	{
		for ( unsigned i = 0; i < rt.SliceCount; ++i )
		{
			SAFE_RELEASE( rt.UAVSlices[i] );
		}

		SAFE_DELETE_ARRAY( rt.UAVSlices );
	}

	rt = RenderTarget();
}

DepthBuffer Direct3D11::CreateDepthBuffer(
	DXGI_FORMAT format,
	unsigned width,
	unsigned height,
	bool sampleInShader,
	unsigned flags,
	unsigned arraySize,
	unsigned mipLevels,
	unsigned sampleCount )
{
	DepthBuffer depthBuffer;
	depthBuffer.FormatDSV = depthBuffer.FormatTex = format;

	if ( sampleInShader )
	{
		switch ( depthBuffer.FormatDSV )
		{
		case DXGI_FORMAT_D16_UNORM:
			depthBuffer.FormatTex = DXGI_FORMAT_R16_TYPELESS;
			depthBuffer.FormatSRV = DXGI_FORMAT_R16_UNORM;
			break;

		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			depthBuffer.FormatTex = DXGI_FORMAT_R24G8_TYPELESS;
			depthBuffer.FormatSRV = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			break;

		case DXGI_FORMAT_D32_FLOAT:
			depthBuffer.FormatTex = DXGI_FORMAT_R32_TYPELESS;
			depthBuffer.FormatSRV = DXGI_FORMAT_R32_FLOAT;
			break;

		default:
			TraceDebug( "Invalid depth format." );
			return DepthBuffer();
		}
	}

	// Create the underlying resource.

	// 1D
	if ( height == 0 )
	{
		D3D11_TEXTURE1D_DESC texDesc;
		texDesc.Width = width;
		texDesc.MipLevels = mipLevels;
		texDesc.ArraySize = arraySize;
		texDesc.Format = depthBuffer.FormatTex;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if ( sampleInShader )
			texDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

		HRESULT hr = _d3dDevice->CreateTexture1D( &texDesc, nullptr, (ID3D11Texture1D**)&depthBuffer.Texture );
		if ( FAILED( hr ) )
		{
			TraceDebug( "Failed to create 1D depth buffer texture" );
			TraceHR( hr );
			return DepthBuffer();
		}

		depthBuffer.SliceCount = texDesc.ArraySize;
	}
	// 2D
	else
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = mipLevels;
		texDesc.ArraySize = arraySize * ((flags & TEXTURE_CUBE) ? 6 : 1);
		texDesc.Format = depthBuffer.FormatTex;
		texDesc.SampleDesc.Count = sampleCount;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if ( sampleInShader )
			texDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

		if ( flags & TEXTURE_CUBE )
			texDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

		HRESULT hr = _d3dDevice->CreateTexture2D( &texDesc, nullptr, (ID3D11Texture2D**)&depthBuffer.Texture );
		if ( FAILED( hr ) )
		{
			TraceDebug( "Failed to create 2D depth buffer texture" );
			TraceHR( hr );
			return DepthBuffer();
		}

		depthBuffer.SliceCount = texDesc.ArraySize;
	}

	// Create views to texture.

	depthBuffer.DSV = _CreateDSV( depthBuffer.Texture, depthBuffer.FormatDSV );
	depthBuffer.DSVReadOnly = _CreateDSV( depthBuffer.Texture, depthBuffer.FormatDSV, true );
	depthBuffer.SRV = _CreateSRV( depthBuffer.Texture, depthBuffer.FormatSRV );

	depthBuffer.DSVSlices = new ID3D11DepthStencilView *[depthBuffer.SliceCount];
	depthBuffer.DSVReadOnlySlices = new ID3D11DepthStencilView *[depthBuffer.SliceCount];
	depthBuffer.SRVSlices = new ID3D11ShaderResourceView *[depthBuffer.SliceCount];
	for ( unsigned i = 0; i < depthBuffer.SliceCount; ++i )
	{
		depthBuffer.DSVSlices[i] = _CreateDSV( depthBuffer.Texture, depthBuffer.FormatDSV, false, i );
		depthBuffer.DSVReadOnlySlices[i] = _CreateDSV( depthBuffer.Texture, depthBuffer.FormatDSV, true, i );
		depthBuffer.SRVSlices[i] = _CreateSRV( depthBuffer.Texture, depthBuffer.FormatSRV, i );
	}

	// Cache stuff
	depthBuffer.Width = width;
	depthBuffer.Height = height;
	depthBuffer.ArraySize = arraySize;
	depthBuffer.MipLevels = mipLevels;
	depthBuffer.SampleCount = sampleCount;

	return depthBuffer;
}

void Direct3D11::DeleteDepthBuffer( DepthBuffer &db )
{
	SAFE_RELEASE( db.Texture );
	SAFE_RELEASE( db.DSV );
	SAFE_RELEASE( db.DSVReadOnly );
	SAFE_RELEASE( db.SRV );

	if ( db.DSVSlices )
	{
		for ( unsigned i = 0; i < db.SliceCount; ++i )
		{
			SAFE_RELEASE( db.DSVSlices[i] );
		}

		SAFE_DELETE_ARRAY( db.DSVSlices );
	}

	if ( db.DSVReadOnlySlices )
	{
		for ( unsigned i = 0; i < db.SliceCount; ++i )
		{
			SAFE_RELEASE( db.DSVReadOnlySlices[i] );
		}

		SAFE_DELETE_ARRAY( db.DSVReadOnlySlices );
	}

	if ( db.SRVSlices )
	{
		for ( unsigned i = 0; i < db.SliceCount; ++i )
		{
			SAFE_RELEASE( db.SRVSlices[i] );
		}

		SAFE_DELETE_ARRAY( db.SRVSlices );
	}

	db = DepthBuffer();
}

ID3D11RenderTargetView* Direct3D11::_CreateRTV(
	ID3D11Resource *resource,
	DXGI_FORMAT format,
	int firstSlice,
	unsigned numSlices
	)
{
	// Get dimension of resource so that we know what we're working with.
	D3D11_RESOURCE_DIMENSION type;
	resource->GetType( &type );

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;

	switch ( type )
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
	{
		// Get description of underlying texture.
		D3D11_TEXTURE1D_DESC texDesc;
		((ID3D11Texture1D*)resource)->GetDesc( &texDesc );

		rtvDesc.Format = (format == DXGI_FORMAT_UNKNOWN) ? texDesc.Format : format;

		// Texture array
		if ( texDesc.ArraySize > 1 )
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
			rtvDesc.Texture1DArray.MipSlice = 0;

			// firstSlice == -1 represents a view to the entire array.
			unsigned theFirstSlice = (firstSlice == -1) ? 0 : firstSlice;
			unsigned theSliceCount = (firstSlice == -1) ? texDesc.ArraySize : numSlices;

			rtvDesc.Texture1DArray.FirstArraySlice = theFirstSlice;
			rtvDesc.Texture1DArray.ArraySize = theSliceCount;
		}
		// Regular texture
		else
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
			rtvDesc.Texture1D.MipSlice = 0;
		}

		break;
	}

	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
	{
		// Get description of underlying texture.
		D3D11_TEXTURE2D_DESC texDesc;
		((ID3D11Texture2D*)resource)->GetDesc( &texDesc );

		rtvDesc.Format = (format == DXGI_FORMAT_UNKNOWN) ? texDesc.Format : format;

		// Texture array
		if ( texDesc.ArraySize > 1 )
		{
			// firstSlice == -1 represents a view to the entire array.
			unsigned theFirstSlice = (firstSlice == -1) ? 0 : firstSlice;
			unsigned theSliceCount = (firstSlice == -1) ? texDesc.ArraySize : numSlices;

			// Multi sampled texture array
			if ( texDesc.SampleDesc.Count > 1 )
			{
				rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
				rtvDesc.Texture2DMSArray.FirstArraySlice = theFirstSlice;
				rtvDesc.Texture2DMSArray.ArraySize = theSliceCount;
			}
			// Regular texture array
			else
			{
				rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
				rtvDesc.Texture2DArray.MipSlice = 0;
				rtvDesc.Texture2DArray.FirstArraySlice = theFirstSlice;
				rtvDesc.Texture2DArray.ArraySize = theSliceCount;
			}
		}
		// Regular texture
		else
		{
			// Multi sampled texture
			if ( texDesc.SampleDesc.Count > 1 )
			{
				rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
			}
			// Regular texture array
			else
			{
				rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				rtvDesc.Texture2D.MipSlice = 0;
			}
		}

		break;
	}

	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
	{
		// Get description of underlying texture.
		D3D11_TEXTURE3D_DESC texDesc;
		((ID3D11Texture3D*)resource)->GetDesc( &texDesc );

		rtvDesc.Format = (format == DXGI_FORMAT_UNKNOWN) ? texDesc.Format : format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
		rtvDesc.Texture3D.MipSlice = 0;

		// firstSlice == -1 represents a view to the entire volume.
		unsigned theFirstSlice = (firstSlice == -1) ? 0 : firstSlice;
		unsigned theSliceCount = (firstSlice == -1) ? texDesc.Depth : numSlices;

		rtvDesc.Texture3D.FirstWSlice = theFirstSlice;
		rtvDesc.Texture3D.WSize = theSliceCount;

		break;
	}

	default:
		TraceDebug( "There's no implementation for resource dimension %i", type );
		return nullptr;
	}

	// RTV description complete; time to create it.
	ID3D11RenderTargetView *rtv = nullptr;
	HRESULT hr = _d3dDevice->CreateRenderTargetView( resource, &rtvDesc, &rtv );
	if ( FAILED( hr ) )
	{
		TraceDebug( "Failed to create render target view" );
		TraceHR( hr );
		return nullptr;
	}

	return rtv;
}

// Note that the function is somewhat misleading; I can't create a SRV to a part
// of a 3D texture for instance...
ID3D11ShaderResourceView* Direct3D11::_CreateSRV(
	ID3D11Resource *resource,
	DXGI_FORMAT format,
	int firstSlice,
	unsigned numSlices )
{
	// Get dimension of resource so that we know what we're working with.
	D3D11_RESOURCE_DIMENSION type;
	resource->GetType( &type );

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	switch ( type )
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
	{
		// Get description of underlying texture.
		D3D11_TEXTURE1D_DESC texDesc;
		((ID3D11Texture1D*)resource)->GetDesc( &texDesc );

		srvDesc.Format = (format == DXGI_FORMAT_UNKNOWN) ? texDesc.Format : format;

		// Texture array
		if ( texDesc.ArraySize > 1 )
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
			srvDesc.Texture1DArray.MostDetailedMip = 0;
			srvDesc.Texture1DArray.MipLevels = texDesc.MipLevels;

			// firstSlice == -1 represents a view to the entire array.
			unsigned theFirstSlice = (firstSlice == -1) ? 0 : firstSlice;
			unsigned theSliceCount = (firstSlice == -1) ? texDesc.ArraySize : numSlices;

			srvDesc.Texture1DArray.FirstArraySlice = theFirstSlice;
			srvDesc.Texture1DArray.ArraySize = theSliceCount;
		}
		// Regular texture
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
			srvDesc.Texture1D.MostDetailedMip = 0;
			srvDesc.Texture1D.MipLevels = texDesc.MipLevels;
		}

		break;
	}

	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
	{
		// Get description of underlying texture.
		D3D11_TEXTURE2D_DESC texDesc;
		((ID3D11Texture2D*)resource)->GetDesc( &texDesc );

		srvDesc.Format = (format == DXGI_FORMAT_UNKNOWN) ? texDesc.Format : format;

		// firstSlice == -1 represents a view to the entire array.
		unsigned theFirstSlice = (firstSlice == -1) ? 0 : firstSlice;
		unsigned theSliceCount = (firstSlice == -1) ? texDesc.ArraySize : numSlices;

		// Texture array
		if ( texDesc.ArraySize > 1 )
		{
			// Texture cube
			if ( texDesc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE )
			{
				// Single cube
				if ( texDesc.ArraySize == 6 )
				{
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
					srvDesc.TextureCube.MostDetailedMip = 0;
					srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
				}
				// Cube array
				else
				{
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
					srvDesc.TextureCubeArray.MostDetailedMip = 0;
					srvDesc.TextureCubeArray.MipLevels = texDesc.MipLevels;
					srvDesc.TextureCubeArray.First2DArrayFace = theFirstSlice;
					srvDesc.TextureCubeArray.NumCubes = (firstSlice == -1) ? texDesc.ArraySize / 6 : numSlices;
				}
			}
			// Multi sampled texture array
			else if ( texDesc.SampleDesc.Count > 1 )
			{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
				srvDesc.Texture2DMSArray.FirstArraySlice = theFirstSlice;
				srvDesc.Texture2DMSArray.ArraySize = theSliceCount;
			}
			// Regular texture array
			else
			{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				srvDesc.Texture2DArray.MostDetailedMip = 0;
				srvDesc.Texture2DArray.MipLevels = texDesc.MipLevels;
				srvDesc.Texture2DArray.FirstArraySlice = theFirstSlice;
				srvDesc.Texture2DArray.ArraySize = theSliceCount;
			}
		}
		// Regular texture
		else
		{
			// Multi sampled texture
			if ( texDesc.SampleDesc.Count > 1 )
			{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			}
			// Regular texture array
			else
			{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
			}
		}

		break;
	}

	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
	{
		// Get description of underlying texture.
		D3D11_TEXTURE3D_DESC texDesc;
		((ID3D11Texture3D*)resource)->GetDesc( &texDesc );

		srvDesc.Format = (format == DXGI_FORMAT_UNKNOWN) ? texDesc.Format : format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		srvDesc.Texture3D.MostDetailedMip = 0;
		srvDesc.Texture3D.MipLevels = texDesc.MipLevels;

		break;
	}

	default:
		TraceDebug( "There's no implementation for resource dimension %i", type );
		return nullptr;
	}

	// SRV description complete; time to create it.
	ID3D11ShaderResourceView *srv = nullptr;
	HRESULT hr = _d3dDevice->CreateShaderResourceView( resource, &srvDesc, &srv );
	if ( FAILED( hr ) )
	{
		TraceDebug( "Failed to create shader resource view" );
		TraceHR( hr );
		return nullptr;
	}

	return srv;
}

ID3D11UnorderedAccessView* Direct3D11::_CreateUAV(
	ID3D11Resource *resource,
	DXGI_FORMAT format,
	int firstSlice,
	unsigned numSlices
	)
{
	// Get dimension of resource so that we know what we're working with.
	D3D11_RESOURCE_DIMENSION type;
	resource->GetType( &type );

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;

	switch ( type )
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
	{
		// Get description of underlying texture.
		D3D11_TEXTURE1D_DESC texDesc;
		((ID3D11Texture1D*)resource)->GetDesc( &texDesc );

		uavDesc.Format = (format == DXGI_FORMAT_UNKNOWN) ? texDesc.Format : format;

		// Texture array
		if ( texDesc.ArraySize > 1 )
		{
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
			uavDesc.Texture1DArray.MipSlice = 0;

			// firstSlice == -1 represents a view to the entire array.
			unsigned theFirstSlice = (firstSlice == -1) ? 0 : firstSlice;
			unsigned theSliceCount = (firstSlice == -1) ? texDesc.ArraySize : numSlices;

			uavDesc.Texture1DArray.FirstArraySlice = theFirstSlice;
			uavDesc.Texture1DArray.ArraySize = theSliceCount;
		}
		// Regular texture
		else
		{
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
			uavDesc.Texture1D.MipSlice = 0;
		}

		break;
	}

	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
	{
		// Get description of underlying texture.
		D3D11_TEXTURE2D_DESC texDesc;
		((ID3D11Texture2D*)resource)->GetDesc( &texDesc );

		uavDesc.Format = (format == DXGI_FORMAT_UNKNOWN) ? texDesc.Format : format;

		// firstSlice == -1 represents a view to the entire array.
		unsigned theFirstSlice = (firstSlice == -1) ? 0 : firstSlice;
		unsigned theSliceCount = (firstSlice == -1) ? texDesc.ArraySize : numSlices;

		// Texture array
		if ( texDesc.ArraySize > 1 )
		{
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
			uavDesc.Texture2DArray.MipSlice = 0;
			uavDesc.Texture2DArray.FirstArraySlice = theFirstSlice;
			uavDesc.Texture2DArray.ArraySize = theSliceCount;
		}
		// Regular texture
		else
		{
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.MipSlice = 0;
		}

		break;
	}

	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
	{
		// Get description of underlying texture.
		D3D11_TEXTURE3D_DESC texDesc;
		((ID3D11Texture3D*)resource)->GetDesc( &texDesc );

		uavDesc.Format = (format == DXGI_FORMAT_UNKNOWN) ? texDesc.Format : format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		uavDesc.Texture3D.MipSlice = 0;

		// firstSlice == -1 represents a view to the entire volume.
		unsigned theFirstSlice = (firstSlice == -1) ? 0 : firstSlice;
		unsigned theSliceCount = (firstSlice == -1) ? texDesc.Depth : numSlices;

		uavDesc.Texture3D.FirstWSlice = theFirstSlice;
		uavDesc.Texture3D.WSize = theSliceCount;

		break;
	}

	default:
		TraceDebug( "There's no implementation for resource dimension %i", type );
		return nullptr;
	}

	// UAV description complete; time to create it.
	ID3D11UnorderedAccessView *uav = nullptr;
	HRESULT hr = _d3dDevice->CreateUnorderedAccessView( resource, &uavDesc, &uav );
	if ( FAILED( hr ) )
	{
		TraceDebug( "Failed to create unordered access view" );
		TraceHR( hr );
		return nullptr;
	}

	return uav;
}

ID3D11DepthStencilView* Direct3D11::_CreateDSV(
	ID3D11Resource *resource,
	DXGI_FORMAT format,
	bool readOnly,
	int firstSlice,
	unsigned numSlices
	)
{
	// Get dimension of resource so that we know what we're working with.
	D3D11_RESOURCE_DIMENSION type;
	resource->GetType( &type );

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;

	// Read only stencil depends on format
	if ( readOnly )
		dsvDesc.Flags |= D3D11_DSV_READ_ONLY_DEPTH;

	switch ( type )
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
	{
		// Get description of underlying texture.
		D3D11_TEXTURE1D_DESC texDesc;
		((ID3D11Texture1D*)resource)->GetDesc( &texDesc );

		dsvDesc.Format = (format == DXGI_FORMAT_UNKNOWN) ? texDesc.Format : format;

		if ( readOnly )
		{
			if ( dsvDesc.Format == DXGI_FORMAT_D24_UNORM_S8_UINT || dsvDesc.Format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT )
				dsvDesc.Flags |= D3D11_DSV_READ_ONLY_STENCIL;
		}

		// Texture array
		if ( texDesc.ArraySize > 1 )
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
			dsvDesc.Texture1DArray.MipSlice = 0;

			// firstSlice == -1 represents a view to the entire array.
			unsigned theFirstSlice = (firstSlice == -1) ? 0 : firstSlice;
			unsigned theSliceCount = (firstSlice == -1) ? texDesc.ArraySize : numSlices;

			dsvDesc.Texture1DArray.FirstArraySlice = theFirstSlice;
			dsvDesc.Texture1DArray.ArraySize = theSliceCount;
		}
		// Regular texture
		else
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
			dsvDesc.Texture1D.MipSlice = 0;
		}

		break;
	}

	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
	{
		// Get description of underlying texture.
		D3D11_TEXTURE2D_DESC texDesc;
		((ID3D11Texture2D*)resource)->GetDesc( &texDesc );

		dsvDesc.Format = (format == DXGI_FORMAT_UNKNOWN) ? texDesc.Format : format;

		if ( readOnly )
		{
			if ( dsvDesc.Format == DXGI_FORMAT_D24_UNORM_S8_UINT || dsvDesc.Format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT )
				dsvDesc.Flags |= D3D11_DSV_READ_ONLY_STENCIL;
		}

		// Texture array
		if ( texDesc.ArraySize > 1 )
		{
			// firstSlice == -1 represents a view to the entire array.
			unsigned theFirstSlice = (firstSlice == -1) ? 0 : firstSlice;
			unsigned theSliceCount = (firstSlice == -1) ? texDesc.ArraySize : numSlices;

			// Multi sampled texture array
			if ( texDesc.SampleDesc.Count > 1 )
			{
				dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
				dsvDesc.Texture2DMSArray.FirstArraySlice = theFirstSlice;
				dsvDesc.Texture2DMSArray.ArraySize = theSliceCount;
			}
			// Regular texture array
			else
			{
				dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
				dsvDesc.Texture2DArray.MipSlice = 0;
				dsvDesc.Texture2DArray.FirstArraySlice = theFirstSlice;
				dsvDesc.Texture2DArray.ArraySize = theSliceCount;
			}
		}
		// Regular texture
		else
		{
			// Multi sampled texture
			if ( texDesc.SampleDesc.Count > 1 )
			{
				dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
			}
			// Regular texture array
			else
			{
				dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = 0;
			}
		}

		break;
	}

	default:
		TraceDebug( "There's no implementation for resource dimension %i", type );
		return nullptr;
	}

	// DSV description complete; time to create it.
	ID3D11DepthStencilView *dsv = nullptr;
	HRESULT hr = _d3dDevice->CreateDepthStencilView( resource, &dsvDesc, &dsv );
	if ( FAILED( hr ) )
	{
		TraceDebug( "Failed to create depth stencil view" );
		TraceHR( hr );
		return nullptr;
	}

	return dsv;
}

StructuredBuffer Direct3D11::CreateStructuredBuffer(
	unsigned stride,
	unsigned elementCount,
	bool CPUWrite,
	bool GPUWrite,
	const void *initialData,
	bool addStructureCounter
	)
{
	StructuredBuffer structuredBuffer;

	// Create the underlying resource.

	D3D11_BUFFER_DESC bufDesc;
	bufDesc.ByteWidth = stride * elementCount;
	bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufDesc.StructureByteStride = stride;

	if ( !CPUWrite && !GPUWrite )
	{
		bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufDesc.CPUAccessFlags = 0;
	}
	else if ( !CPUWrite && GPUWrite )
	{
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		bufDesc.CPUAccessFlags = 0;
	}
	else if ( CPUWrite && !GPUWrite )
	{
		bufDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		TraceDebug( "Cannot create a structured buffer with both CPU write and GPU write access" );
		return StructuredBuffer();
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = initialData;
	initData.SysMemPitch = 0; // Not used for buffers
	initData.SysMemSlicePitch = 0; // Not used for buffers

	HRESULT hr = _d3dDevice->CreateBuffer( &bufDesc, initialData ? &initData : nullptr, &structuredBuffer.Buffer );
	if ( FAILED( hr ) )
	{
		TraceDebug( "Failed to create structured buffer" );
		TraceHR( hr );
		return StructuredBuffer();
	}

	// Create views to the buffer.

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = elementCount;

	hr = _d3dDevice->CreateShaderResourceView( structuredBuffer.Buffer, &srvDesc, &structuredBuffer.SRV );
	if ( FAILED( hr ) )
	{
		TraceDebug( "Failed to create shader resource view" );
		TraceHR( hr );
	}

	if ( GPUWrite )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = elementCount;
		uavDesc.Buffer.Flags = addStructureCounter ? D3D11_BUFFER_UAV_FLAG_COUNTER : 0;

		hr = _d3dDevice->CreateUnorderedAccessView( structuredBuffer.Buffer, &uavDesc, &structuredBuffer.UAV );
		if ( FAILED( hr ) )
		{
			TraceDebug( "Failed to create unordered access view" );
			TraceHR( hr );
		}
	}

	// Cache stuff.
	structuredBuffer.Stride = stride;
	structuredBuffer.ElementCount = elementCount;

	return structuredBuffer;
}

StructuredBuffer Direct3D11::CreateAppendConsumeBuffer(
	unsigned stride,
	unsigned elementCount,
	const void *initialData
	)
{
	StructuredBuffer structuredBuffer;

	// Create the underlying resource.

	D3D11_BUFFER_DESC bufDesc;
	bufDesc.ByteWidth = stride * elementCount;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufDesc.StructureByteStride = stride;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = initialData;
	initData.SysMemPitch = 0; // Not used for buffers
	initData.SysMemSlicePitch = 0; // Not used for buffers

	HRESULT hr = _d3dDevice->CreateBuffer( &bufDesc, initialData ? &initData : nullptr, &structuredBuffer.Buffer );
	if ( FAILED( hr ) )
	{
		TraceDebug( "Failed to create structured buffer" );
		TraceHR( hr );
		return StructuredBuffer();
	}

	// Create views to the buffer.

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = elementCount;

	hr = _d3dDevice->CreateShaderResourceView( structuredBuffer.Buffer, &srvDesc, &structuredBuffer.SRV );
	if ( FAILED( hr ) )
	{
		TraceDebug( "Failed to create shader resource view" );
		TraceHR( hr );
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = elementCount;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	hr = _d3dDevice->CreateUnorderedAccessView( structuredBuffer.Buffer, &uavDesc, &structuredBuffer.UAV );
	if ( FAILED( hr ) )
	{
		TraceDebug( "Failed to create unordered access view" );
		TraceHR( hr );
	}

	// Cache stuff.
	structuredBuffer.Stride = stride;
	structuredBuffer.ElementCount = elementCount;

	return structuredBuffer;
}

void Direct3D11::DeleteStructuredBuffer( StructuredBuffer &sb )
{
	SAFE_RELEASE( sb.Buffer );
	SAFE_RELEASE( sb.SRV );
	SAFE_RELEASE( sb.UAV );
	sb = StructuredBuffer();
}

IndirectArgsBuffer Direct3D11::CreateIndirectArgsBuffer(
	unsigned size,
	void *initialData
	)
{
	IndirectArgsBuffer indirectArgs;

	// Create underlying resource.
	D3D11_BUFFER_DESC bufDesc;
	bufDesc.ByteWidth = size;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = 0;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	bufDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = initialData;
	initData.SysMemPitch = 0; // Not used for buffers
	initData.SysMemSlicePitch = 0; // Not used for buffers

	HRESULT hr = _d3dDevice->CreateBuffer( &bufDesc, initialData ? &initData : nullptr, &indirectArgs.Buffer );
	if ( FAILED( hr ) )
	{
		TraceDebug( "Failed to create indirect args buffer" );
		TraceHR( hr );
		return IndirectArgsBuffer();
	}

	// Cache stuff
	indirectArgs.Size = size;

	return indirectArgs;
}

void Direct3D11::DeleteIndirectArgsBuffer( IndirectArgsBuffer &iab )
{
	SAFE_RELEASE( iab.Buffer );
	iab = IndirectArgsBuffer();
}