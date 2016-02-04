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
#include "ILightProvider.h"
#include "ShaderData.h"
#include "ILightProvider.h"
#include "GPUTimer.h"
#include "ITextProvider.h"

using namespace std;

class Graphics
{
public:
	Graphics();
	~Graphics();

	const void Init();
	const void Shutdown();

	void Render(double totalTime, double deltaTime);
	const void ResizeSwapChain();

	void AddRenderProvider(IRenderProvider *provider);
	void AddCameraProvider(ICameraProvider* provider);
	void AddOverlayProvider(IOverlayProvider* provider);
	void AddLightProvider(ILightProvider* provider);
	void AddTextProvider(ITextProvider* provider);

	const void ClearRenderProviders();
	const void ClearOverlayProviders();
	const void ClearCameraProviders();
	const void ClearLightProviders();
	const void ClearTextProviders();

	bool CreateMeshBuffers(Mesh *mesh, std::uint32_t& vertexBufferIndex, std::uint32_t& indexBufferIndex);
	uint CreateTextBuffer(FontData& data);
	const void UpdateTextBuffer(uint buffer, FontData& data);
	ShaderData GenerateMaterial(const wchar_t *shaderFile);
	std::int32_t CreateTexture(const wchar_t *filename);

	ID3D11Device* GetDevice()const;
	ID3D11DeviceContext* GetDeviceContext()const;


private:
	struct StaticMeshVSConstants
	{
		DirectX::XMFLOAT4X4 WVP;
		DirectX::XMFLOAT4X4 WorldViewInvTrp;
	};
	struct TextConstants
	{
		DirectX::XMFLOAT4X4 Ortho;
		DirectX::XMFLOAT4 Color;
	};

	struct TiledDeferredConstants
	{
		DirectX::XMFLOAT4X4 View;
		DirectX::XMFLOAT4X4 Proj;
		DirectX::XMFLOAT4X4 InvView;
		DirectX::XMFLOAT4X4 InvProj;
		float BackbufferWidth;
		float BackbufferHeight;
		int PointLightCount;
		int SpotLightCount;
		int CapsuleLightCount;
		float pad1;
		float pad2;
		float pad3;
	};

private:
	HRESULT OnCreateDevice(void);
	void OnDestroyDevice(void);
	HRESULT OnResizedSwapChain(void);
	void OnReleasingSwapChain(void);

	void BeginFrame(void);
	void EndFrame(void);

	void _InterleaveVertexData(Mesh *mesh, void **vertexData, std::uint32_t& vertexDataSize, void **indexData, std::uint32_t& indexDataSize);
	ID3D11Buffer* _CreateVertexBuffer(void *vertexData, std::uint32_t vertexDataSize);
	ID3D11Buffer* _CreateDynamicVertexBuffer(void *vertexData, std::uint32_t vertexDataSize);
	const void _BuildVertexData(FontData& data, TextVertexLayout*& vertexPtr, uint32_t& vertexDataSize);
	ID3D11Buffer* _CreateIndexBuffer(void *indexData, std::uint32_t indexDataSize);

	bool _BuildInputLayout(void);
	bool _BuildTextInputLayout(void);
	void _EnsureMinimumMaterialCBSize(std::uint32_t size);

	void _RenderLightsTiled(ID3D11DeviceContext *deviceContext, double totalTime);

private:
	Direct3D11 *_D3D11 = nullptr;

	std::vector<IRenderProvider*> _RenderProviders;
	std::vector<ICameraProvider*> _cameraProviders;
	std::vector<IOverlayProvider*> _overlayProviders;
	std::vector<ILightProvider*> _lightProviders;
	std::vector<ITextProvider*> _textProviders;

	// Elements are submitted by render providers, and is cleared on every
	// frame. It's a member variable to avoid reallocating memory every frame.
	RenderJobMap _renderJobs;
	std::vector<OverlayData> _overlayRenderJobs;
	CamData _renderCamera;
	TextJob2 _textJobs;

	std::vector<PointLight> _pointLights;
	std::vector<SpotLight> _spotLights;
	std::vector<CapsuleLight> _capsuleLights;

	StructuredBuffer _pointLightsBuffer;
	StructuredBuffer _spotLightsBuffer;
	StructuredBuffer _capsuleLightsBuffer;

	const void _GatherRenderData();
	const void _RenderMeshes();
	const void _RenderOverlays()const;
	const void _RenderTexts();
	const void _RenderGBuffers(uint numImages)const;

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

	ID3D11ComputeShader* _tiledDeferredCS = nullptr;
	ID3D11Buffer* _tiledDeferredConstants = nullptr;

	RenderTarget _accumulateRT;

	ID3D11VertexShader *_fullscreenTextureVS = nullptr;
	ID3D11PixelShader *_fullscreenTexturePSMultiChannel = nullptr;
	ID3D11PixelShader *_fullscreenTexturePSSingleChannel = nullptr;

	ID3D11VertexShader* _textVSShader = nullptr;
	ID3D11PixelShader* _textPSShader = nullptr;
	ID3D11InputLayout* _textInputLayot = nullptr;
	ID3D10Blob* _textShaderInput = nullptr;
	DirectX::XMFLOAT4X4 _orthoMatrix;
	ID3D11Buffer* _textConstantBuffer = nullptr;

	ID3D11SamplerState *_triLinearSam = nullptr;


	GPUTimer timer;
};

#endif