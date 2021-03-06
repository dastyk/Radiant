#ifndef _GBUFFER_H_
#define _GBUFFER_H_

#include <d3d11.h>
#include <cstdint>

class GBuffer
{
public:
	GBuffer( ID3D11Device *device, std::uint32_t width, std::uint32_t height );
	~GBuffer( void );

	void Clear( ID3D11DeviceContext *deviceContext );
	std::uint32_t NumBuffers( void ) const { return _numBuffers; }
	ID3D11ShaderResourceView *ColorSRV( void ) const { return _colorSRV; }
	ID3D11RenderTargetView *ColorRT( void ) const { return _colorRT; }
	ID3D11ShaderResourceView *NormalSRV( void ) const { return _normalSRV; }
	ID3D11RenderTargetView *NormalRT( void ) const { return _normalRT; }
	ID3D11ShaderResourceView *EmissiveSRV( void ) const { return _emissiveSRV; }
	ID3D11RenderTargetView *EmissiveRT( void ) const { return _emissiveRT; }
	ID3D11ShaderResourceView *DepthSRV(void) const { return _depthSRV; }
	ID3D11RenderTargetView *DepthRT(void) const { return _depthRT; }
	ID3D11ShaderResourceView *LightSRV(void) const { return _lightSRV; }
	ID3D11RenderTargetView *LightRT(void) const { return _lightRT; }
	ID3D11ShaderResourceView *LightFinSRV(void) const { return _lightFinSRV; }
	ID3D11RenderTargetView *LightFinRT(void) const { return _lightFinRT; }
	ID3D11ShaderResourceView *LightVolSRV(void) const { return _lightVolSRV; }
	ID3D11RenderTargetView *LightVolRT(void) const { return _lightVolRT; }

private:
	GBuffer( const GBuffer& rhs );
	GBuffer &operator=( const GBuffer& rhs );

private:
	std::uint32_t _width;
	std::uint32_t _height;
	std::uint32_t _numBuffers;

	ID3D11RenderTargetView *_colorRT = nullptr;
	ID3D11ShaderResourceView *_colorSRV = nullptr;
	ID3D11RenderTargetView *_normalRT = nullptr;
	ID3D11ShaderResourceView *_normalSRV = nullptr;
	ID3D11RenderTargetView *_emissiveRT = nullptr;
	ID3D11ShaderResourceView *_emissiveSRV = nullptr;

	ID3D11RenderTargetView * _depthRT = nullptr;
	ID3D11ShaderResourceView *_depthSRV = nullptr;

	ID3D11RenderTargetView * _lightRT = nullptr;
	ID3D11ShaderResourceView *_lightSRV = nullptr;

	ID3D11RenderTargetView * _lightFinRT = nullptr;
	ID3D11ShaderResourceView *_lightFinSRV = nullptr;

	ID3D11RenderTargetView * _lightVolRT = nullptr;
	ID3D11ShaderResourceView *_lightVolSRV = nullptr;


	ID3D11VertexShader *_clearGBufferVS = nullptr;
	ID3D11PixelShader *_clearGBufferPS = nullptr;
};

#endif // _GBUFFER_H_