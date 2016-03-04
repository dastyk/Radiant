#include "GBuffer.h"
#include "Utils.h"
#include "Shader.h"

using namespace std;

// It might be the responsibility of this class to visualize the buffers.
// GBuffer::Visualize could render either a full screen of the buffers or smaller
// squares.

GBuffer::GBuffer( ID3D11Device *device, uint32_t width, uint32_t height ) :
	_width( width ),
	_height( height )
{
	_numBuffers = 3;

	ID3D11Texture2D *tex;
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.Height = _height;
	texDesc.Width = _width;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	// Color buffer
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	HR( device->CreateTexture2D( &texDesc, nullptr, &tex ) );
	HR( device->CreateShaderResourceView( tex, nullptr, &_colorSRV ) );
	HR( device->CreateRenderTargetView( tex, nullptr, &_colorRT ) );
	// Views save reference
	SAFE_RELEASE( tex );

	// Normal buffer
	
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// DXGI_FORMAT_R16G16B16A16_FLOAT;
	HR( device->CreateTexture2D( &texDesc, nullptr, &tex ) );
	HR( device->CreateShaderResourceView( tex, nullptr, &_normalSRV ) );
	HR( device->CreateRenderTargetView( tex, nullptr, &_normalRT ) );
	// Views save reference
	SAFE_RELEASE( tex );

	// Emissive buffer
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	HR( device->CreateTexture2D( &texDesc, nullptr, &tex ) );
	HR( device->CreateShaderResourceView( tex, nullptr, &_emissiveSRV ) );
	HR( device->CreateRenderTargetView( tex, nullptr, &_emissiveRT ) );
	// Views save reference
	SAFE_RELEASE( tex );


	// Light buffer

	texDesc.Format = DXGI_FORMAT_R16_FLOAT;
	HR(device->CreateTexture2D(&texDesc, nullptr, &tex));
	HR(device->CreateShaderResourceView(tex, nullptr, &_depthSRV));
	HR(device->CreateRenderTargetView(tex, nullptr, &_depthRT));
	// Views save reference
	SAFE_RELEASE(tex);

	// Light buffer
	
	texDesc.Format = DXGI_FORMAT_R16_FLOAT;
	HR(device->CreateTexture2D(&texDesc, nullptr, &tex));
	HR(device->CreateShaderResourceView(tex, nullptr, &_lightSRV));
	HR(device->CreateRenderTargetView(tex, nullptr, &_lightRT));
	// Views save reference
	SAFE_RELEASE(tex);

	// Light buffer
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	HR(device->CreateTexture2D(&texDesc, nullptr, &tex));
	HR(device->CreateShaderResourceView(tex, nullptr, &_lightFinSRV));
	HR(device->CreateRenderTargetView(tex, nullptr, &_lightFinRT));
	// Views save reference
	SAFE_RELEASE(tex);

	// Light vol buffer
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	HR(device->CreateTexture2D(&texDesc, nullptr, &tex));
	HR(device->CreateShaderResourceView(tex, nullptr, &_lightVolSRV));
	HR(device->CreateRenderTargetView(tex, nullptr, &_lightVolRT));
	// Views save reference
	SAFE_RELEASE(tex);

	
	_clearGBufferVS = CompileVSFromFile( device, L"Shaders/ClearGBuffer.hlsl", "VS", "vs_5_0" );
	_clearGBufferPS = CompilePSFromFile( device, L"Shaders/ClearGBuffer.hlsl", "PS", "ps_5_0" );
}

GBuffer::~GBuffer()
{
	SAFE_RELEASE( _colorSRV );
	SAFE_RELEASE( _colorRT );
	SAFE_RELEASE( _normalSRV );
	SAFE_RELEASE( _normalRT );
	SAFE_RELEASE( _emissiveSRV );
	SAFE_RELEASE( _emissiveRT );
	SAFE_RELEASE(_depthSRV);
	SAFE_RELEASE(_depthRT);
	SAFE_RELEASE(_lightSRV);
	SAFE_RELEASE(_lightRT);
	SAFE_RELEASE(_lightFinSRV);
	SAFE_RELEASE(_lightFinRT);
	SAFE_RELEASE(_lightVolSRV);
	SAFE_RELEASE(_lightVolRT);
	SAFE_RELEASE( _clearGBufferVS );
	SAFE_RELEASE( _clearGBufferPS );
}

// Render a full-screen quad using a shader that clears the GBuffer to defaults.
void GBuffer::Clear( ID3D11DeviceContext *deviceContext )
{
	// Clearing the GBuffer does not require any depth test.
	ID3D11RenderTargetView *rtvs[] = { _colorRT, _normalRT, _emissiveRT, _lightRT, _lightFinRT, _depthRT, _lightVolRT };
	deviceContext->OMSetRenderTargets(_numBuffers, rtvs, nullptr );

	deviceContext->VSSetShader( _clearGBufferVS, nullptr, 0 ); // Why draw a fullscreen quad? why not just use 	deviceContext->ClearRenderTargetView()?
	deviceContext->PSSetShader( _clearGBufferPS, nullptr, 0 );
	deviceContext->Draw( 3, 0 );
}