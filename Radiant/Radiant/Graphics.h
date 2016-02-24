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
#include "IDecalProvider.h"
#include "GPUTimer.h"
#include "CPUTimer.h"
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
	void AddDecalProvider(IDecalProvider* provider);

	const void ClearRenderProviders();
	const void ClearOverlayProviders();
	const void ClearCameraProviders();
	const void ClearLightProviders();
	const void ClearTextProviders();
	const void ClearDecalProviders();


	void ReleaseVertexBuffer(uint32_t vertexBufferIndex);
	void ReleaseIndexBuffer(uint32_t indexBufferIndex);
	void ReleaseStaticMeshBuffers(const std::vector<uint32_t>& vbIndices, const std::vector<uint32_t>& ibIndices);
	const void ReleaseTexture(uint32_t textureID);
	const void ReleaseDynamicVertexBuffer(uint buffer);
	bool CreateMeshBuffers(Mesh *mesh, std::uint32_t& vertexBufferIndex, std::uint32_t& indexBufferIndex);
	uint CreateTextBuffer(FontData* data);
	const void UpdateTextBuffer(FontData* data);
	ShaderData GenerateMaterial(const wchar_t *shaderFile);
	std::int32_t CreateTexture(const wchar_t *filename);

	ID3D11Device* GetDevice()const;
	ID3D11DeviceContext* GetDeviceContext()const;

	std::string GetAVGTPFTimes();


private:
	struct OncePerFrameConstants
	{
		DirectX::XMFLOAT4X4 View;
		DirectX::XMFLOAT4X4 Proj;
		DirectX::XMFLOAT4X4 ViewProj;
		DirectX::XMFLOAT4X4 InvView;
		DirectX::XMFLOAT4X4 InvProj;
		DirectX::XMFLOAT4X4 InvViewProj;
		DirectX::XMFLOAT4X4 Ortho;
		DirectX::XMFLOAT3 CameraPosition;
		float DrawDistance;
		float BackbufferWidth;
		float BackbufferHeight;
		DirectX::XMFLOAT2 pad = DirectX::XMFLOAT2(0.0f, 0.0f);
	};
	struct StaticMeshVSConstants
	{
		DirectX::XMFLOAT4X4 WVP;
		DirectX::XMFLOAT4X4 WorldViewInvTrp;
		DirectX::XMFLOAT4X4 World;
		DirectX::XMFLOAT4X4 WorldView;
	};
	struct TextPSConstants
	{
		DirectX::XMFLOAT4 Color;
	};
	struct TiledDeferredConstants
	{
		int PointLightCount;
		int SpotLightCount;
		int CapsuleLightCount;
		int AreaRectLightCount;
	};
	struct DynamicVertexBuffer
	{
		ID3D11Buffer* buffer = nullptr;
		uint size;

		DynamicVertexBuffer(): buffer(nullptr), size(0)
		{

		}
		DynamicVertexBuffer(const DynamicVertexBuffer& other) : buffer(other.buffer), size(other.size)
		{
		}
		DynamicVertexBuffer& operator=(const DynamicVertexBuffer& other)
		{
			buffer = other.buffer;
			size = other.size;
			return *this;
		}
	};

	struct PointLightData
	{
		Mesh* mesh = nullptr;
		uint vertexbuffer;
		uint indexBuffer;
		uint indexCount;
		ID3D11Buffer* constantBuffer = nullptr;
	};
	typedef PointLightData DecalData;

	struct DecalsPerObjectBuffer
	{
		DirectX::XMFLOAT4X4 invWorld;
	};
	struct DecalsVSConstantBuffer
	{
		DirectX::XMFLOAT4X4 WorldViewProj;
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
	const DynamicVertexBuffer _CreateDynamicVertexBuffer(void *vertexData, std::uint32_t vertexDataSize)const;
	const void _DeleteDynamicVertexBuffer(DynamicVertexBuffer& buffer)const;
	const bool _ResizeDynamicVertexBuffer(DynamicVertexBuffer& buffer, void *vertexData, std::uint32_t& vertexDataSize)const;
	const void _MapDataToDynamicVertexBuffer(DynamicVertexBuffer& buffer, void *vertexData, std::uint32_t vertexDataSize)const;
	const void _BuildVertexData(FontData* data, TextVertexLayout** vertexPtr, uint32_t& vertexDataSize);
	ID3D11Buffer* _CreateIndexBuffer(void *indexData, std::uint32_t indexDataSize);

	bool _BuildInputLayout(void);
	bool _BuildTextInputLayout(void);
	bool _BuildLightInputLayout(void);
	void _EnsureMinimumMaterialCBSize(std::uint32_t size);

	void _RenderLightsTiled(ID3D11DeviceContext *deviceContext, double totalTime);
	void _RenderLights();

	const void _SetOncePerFrameBuffer();
	const void _GatherRenderData();
	const void _RenderMeshes();
	const void _RenderOverlays()const;
	const void _RenderDecals();
	const void _RenderTexts();
	const void _RenderGBuffers(uint numImages)const;
	void _GenerateGlow();

	const PointLightData _CreatePointLightData(unsigned detail);
	const void _DeletePointLightData(PointLightData& geo)const;
	
	Graphics::DecalData _createDecalData(); // Used for decals
	void _deleteDecalData(DecalData& dd);
	
private:
	ID3D11Buffer* _oncePerFrameConstantsBuffer;


	Direct3D11 *_D3D11 = nullptr;

	std::vector<IRenderProvider*> _RenderProviders;
	std::vector<ICameraProvider*> _cameraProviders;
	std::vector<IOverlayProvider*> _overlayProviders;
	std::vector<ILightProvider*> _lightProviders;
	std::vector<ITextProvider*> _textProviders;
	std::vector<IDecalProvider*> _decalProviders;

	// Elements are submitted by render providers, and is cleared on every
	// frame. It's a member variable to avoid reallocating memory every frame.
	RenderJobMap _renderJobs;
	std::vector<OverlayData*> _overlayRenderJobs;
	CameraData* _renderCamera;
	TextJob _textJobs;

	std::vector<PointLight*> _pointLights;
	std::vector<SpotLight*> _spotLights;
	std::vector<CapsuleLight*> _capsuleLights;
	std::vector<AreaRectLight*> _areaRectLights;

	StructuredBuffer _pointLightsBuffer;
	StructuredBuffer _spotLightsBuffer;
	StructuredBuffer _capsuleLightsBuffer;
	StructuredBuffer _areaRectLightBuffer;

	std::vector<Decal*> _decals;
	

	std::vector<ID3D11Buffer*> _VertexBuffers;
	std::vector<ID3D11Buffer*> _IndexBuffers;
	std::vector<ID3D11VertexShader*> _VertexShaders;
	std::vector<ID3D11InputLayout*> _inputLayouts;
	std::vector<ID3D11PixelShader*> _pixelShaders;
	std::vector<DynamicVertexBuffer> _DynamicVertexBuffers;


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

	RenderTarget _glowTempRT1;
	RenderTarget _glowTempRT2;
	ID3D11PixelShader *_separableBlurHorizontal = nullptr;
	ID3D11PixelShader *_separableBlurVertical = nullptr;
	ID3D11Buffer *_blurTexelConstants = nullptr;

	ID3D11ComputeShader* _tiledDeferredCS = nullptr;
	ID3D11Buffer* _tiledDeferredConstants = nullptr;

	RenderTarget _accumulateRT;

	ID3D11VertexShader *_fullscreenTextureVS = nullptr;
	ID3D11PixelShader *_fullscreenTexturePSMultiChannel = nullptr;
	ID3D11PixelShader *_fullscreenTexturePSSingleChannel = nullptr;
	ID3D11PixelShader *_fullscreenTexturePSMultiChannelGamma = nullptr;
	ID3D11PixelShader *_downSamplePS = nullptr;

	ID3D11VertexShader* _textVSShader = nullptr;
	ID3D11PixelShader* _textPSShader = nullptr;
	ID3D11InputLayout* _textInputLayout = nullptr;
	ID3D10Blob* _textShaderInput = nullptr;
	DirectX::XMFLOAT4X4 _orthoMatrix;
	ID3D11Buffer* _textPSConstantBuffer = nullptr;

	DecalData _DecalData;
	ID3D11Buffer* _decalsVSConstants = nullptr;
	//ID3D11Buffer* _decalsPSConstants = nullptr;
	ID3D11VertexShader* _decalsVSShader = nullptr;
	ID3D11PixelShader* _decalsPSShader = nullptr;

	ID3D11SamplerState *_anisoSam = nullptr;
	ID3D11SamplerState *_triLinearSam = nullptr;

	GPUTimer timer;
	CPUTimer ctimer;

	PointLightData _PointLightData;
	ID3D11VertexShader* _lightVertexShader = nullptr;
	ID3D11PixelShader* _lightPixelShader = nullptr;
	ID3D11PixelShader* _lightFinalPixelShader = nullptr;
	ID3D11InputLayout* _lightInputLayout = nullptr;
	ID3D10Blob* _lightShaderInput = nullptr;
	DepthStencilState _dssWriteToDepthDisabled;
	DepthStencilState _dssWriteToDepthEnabled;
	RasterizerState _rsBackFaceCullingEnabled;
	RasterizerState _rsFrontFaceCullingEnabled;
	RasterizerState _rsFaceCullingDisabled;
	BlendState _bsBlendEnabled;
	BlendState _bsBlendDisabled;
};

#endif