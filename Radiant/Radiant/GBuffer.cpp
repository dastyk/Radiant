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
	_numBuffers = 2;

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
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	HR( device->CreateTexture2D( &texDesc, nullptr, &tex ) );
	HR( device->CreateShaderResourceView( tex, nullptr, &_normalSRV ) );
	HR( device->CreateRenderTargetView( tex, nullptr, &_normalRT ) );
	// Views save reference
	SAFE_RELEASE( tex );

	_clearGBufferVS = CompileVSFromFile( device, L"Shaders/ClearGBuffer.hlsl", "VS", "vs_5_0" );
	_clearGBufferPS = CompilePSFromFile( device, L"Shaders/ClearGBuffer.hlsl", "PS", "ps_5_0" );
}

GBuffer::~GBuffer()
{
	SAFE_RELEASE( _colorSRV );
	SAFE_RELEASE( _colorRT );
	SAFE_RELEASE( _normalSRV );
	SAFE_RELEASE( _normalRT );

	SAFE_RELEASE( _clearGBufferVS );
	SAFE_RELEASE( _clearGBufferPS );
}

// Render a full-screen quad using a shader that clears the GBuffer to defaults.
void GBuffer::Clear( ID3D11DeviceContext *deviceContext )
{
	// Clearing the GBuffer does not require any depth test.
	ID3D11RenderTargetView *rtvs[] = { _colorRT, _normalRT };
	deviceContext->OMSetRenderTargets( 2, rtvs, nullptr );

	deviceContext->VSSetShader( _clearGBufferVS, nullptr, 0 );
	deviceContext->PSSetShader( _clearGBufferPS, nullptr, 0 );
	deviceContext->Draw( 3, 0 );
}