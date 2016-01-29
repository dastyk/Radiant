#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

//////////////
// Includes //
//////////////
#include <d3d11.h>
#include <vector>
#include <unordered_map>

#include <DirectXMath.h>

////////////////////
// Local Includes //
////////////////////
#include "Direct3D11.h"
#include "IRenderProvider.h"
#include "ICameraProvider.h"
#include "Mesh.h"
#include "Utils.h"
#include "Shader.h"
#include "GBuffer.h"
#include "IOverlayProvider.h"
#include "ShaderData.h"

using namespace std;

class Graphics
{
public:
	Graphics();
	~Graphics();

	const void Init();
	const void Shutdown();

	void Render( double totalTime, double deltaTime );
	const void ResizeSwapChain();

	void AddRenderProvider( IRenderProvider *provider );
	void AddCameraProvider(ICameraProvider* provider);
	void AddOverlayProvider(IOverlayProvider* provider);

	const void ClearRenderProviders();
	const void ClearOverlayroviders();
	const void ClearCameraProviders();

	bool CreateMeshBuffers( Mesh *mesh, std::uint32_t& vertexBufferIndex, std::uint32_t& indexBufferIndex );
	ShaderData GenerateMaterial( const wchar_t *shaderFile );
	std::int32_t CreateTexture( const wchar_t *filename );

private:
	struct StaticMeshVSConstants
	{
		DirectX::XMFLOAT4X4 WVP;
		DirectX::XMFLOAT4X4 WorldViewInvTrp;
	};

private:
	HRESULT OnCreateDevice( void );
	void OnDestroyDevice( void );
	HRESULT OnResizedSwapChain( void );
	void OnReleasingSwapChain( void );

	void BeginFrame( void );
	void EndFrame( void );

	void _InterleaveVertexData( Mesh *mesh, void **vertexData, std::uint32_t& vertexDataSize, void **indexData, std::uint32_t& indexDataSize );
	ID3D11Buffer* _CreateVertexBuffer( void *vertexData, std::uint32_t vertexDataSize );
	ID3D11Buffer* _CreateIndexBuffer( void *indexData, std::uint32_t indexDataSize );

	bool _BuildInputLayout( void );

	void _EnsureMinimumMaterialCBSize( std::uint32_t size );

private:
	Direct3D11 *_D3D11 = nullptr;

	std::vector<IRenderProvider*> _RenderProviders;
	std::vector<ICameraProvider*> _cameraProviders;
	std::vector<IOverlayProvider*> _overlayProviders;

	// Elements are submitted by render providers, and is cleared on every
	// frame. It's a member variable to avoid reallocating memory every frame.
	RenderJobMap _renderJobs;
	std::vector<OverlayData> _overlayRenderJobs;
	CamData _renderCamera;

	std::vector<ID3D11Buffer*> _VertexBuffers;
	std::vector<ID3D11Buffer*> _IndexBuffers;
	std::vector<ID3D11VertexShader*> _VertexShaders;
	std::vector<ID3D11InputLayout*> _inputLayouts;
	std::vector<ID3D11PixelShader*> _pixelShaders;

	ShaderData _defaultMaterial;
	std::vector<ID3D11PixelShader*> _materialShaders;
	ID3D11Buffer *_materialConstants = nullptr;
	std::uint32_t _currentMaterialCBSize = 0;

	std::vector<ID3D11ShaderResourceView*> _textures;

	DepthBuffer _mainDepth;

	ID3D11InputLayout *_inputLayout = nullptr;
	ID3D11VertexShader *_staticMeshVS = nullptr;
	ID3D11Buffer *_staticMeshVSConstants = nullptr;
	ID3D10Blob *_basicShaderInput = nullptr;

	GBuffer* _GBuffer = nullptr;

	ID3D11VertexShader *_fullscreenTextureVS = nullptr;
	ID3D11PixelShader *_fullscreenTexturePSMultiChannel = nullptr;
	ID3D11PixelShader *_fullscreenTexturePSSingleChannel = nullptr;

	ID3D11SamplerState *_triLinearSam = nullptr;
};

#endif