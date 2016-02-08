/*
Should probably extract the resource stuff into their own specific classes
and let the remaining class simply be a device manager that just creates a swap
chain and whatnot (after all, most of these methods simply fill out a struct
which could be represented by classes in their own right).
*/

#ifndef _DIRECT3D11_H_
#define _DIRECT3D11_H_

#include "Utils.h"
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

enum TextureFlags : unsigned
{
	TEXTURE_COMPUTE_WRITE = 1 << 0,
	TEXTURE_GEN_MIPS = 1 << 1,
	TEXTURE_CUBE = 1 << 2
};

// A render target could potentially hold it's corresponding viewport.
struct RenderTarget
{
	ID3D11Resource *Texture = nullptr;
	ID3D11RenderTargetView *RTV = nullptr;
	ID3D11ShaderResourceView *SRV = nullptr;
	ID3D11UnorderedAccessView *UAV = nullptr;

	// Views to parts of the render target.
	// 1D/2D array: The individual array elements. Number of slices is ArraySize.
	// 3D: The 2D slices along the third dimension. Number of slices is Depth.
	// Cube: The individual faces of the cubes. Number of slices is 6 * ArraySize.
	ID3D11RenderTargetView **RTVSlices = nullptr;
	ID3D11ShaderResourceView **SRVSlices = nullptr;
	ID3D11UnorderedAccessView **UAVSlices = nullptr;
	unsigned SliceCount = 1;

	DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
	unsigned Width = 0;
	unsigned Height = 0;
	unsigned Depth = 0;
	unsigned ArraySize = 1; // Not used for 3D
	unsigned MipLevels = 1;
	unsigned SampleCount = 1; // Not used for 1D and 3D (is it used for Cube?)
};

struct DepthBuffer
{
	ID3D11Resource *Texture = nullptr;
	ID3D11DepthStencilView *DSV = nullptr;
	ID3D11DepthStencilView *DSVReadOnly = nullptr;
	ID3D11ShaderResourceView *SRV = nullptr;

	// Views to parts of the depth buffer if it's an array.
	ID3D11DepthStencilView **DSVSlices = nullptr;
	ID3D11DepthStencilView **DSVReadOnlySlices = nullptr;
	ID3D11ShaderResourceView **SRVSlices = nullptr;
	unsigned SliceCount = 1;

	DXGI_FORMAT FormatTex = DXGI_FORMAT_UNKNOWN;
	DXGI_FORMAT FormatDSV = DXGI_FORMAT_UNKNOWN;
	DXGI_FORMAT FormatSRV = DXGI_FORMAT_UNKNOWN;
	unsigned Width = 0;
	unsigned Height = 0;
	unsigned ArraySize = 1;
	unsigned MipLevels = 1;
	unsigned SampleCount = 1; // Not used for 1D
};

struct StructuredBuffer
{
	ID3D11Buffer *Buffer = nullptr;
	ID3D11ShaderResourceView *SRV = nullptr;
	ID3D11UnorderedAccessView *UAV = nullptr;

	unsigned Stride = 0;
	unsigned ElementCount = 0;
};

struct IndirectArgsBuffer
{
	ID3D11Buffer *Buffer = nullptr;
	unsigned Size = 0;
};

struct DepthStencilState
{
	ID3D11DepthStencilState*	DSS = nullptr;
	bool						depthEnable = true;
	D3D11_DEPTH_WRITE_MASK		depthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	D3D11_COMPARISON_FUNC		depthComparison = D3D11_COMPARISON_LESS;
	bool						stencilEnable = false;
	UINT8						stencilReadMask = 0xFF;
	UINT8						stencilWriteMask = 0xFF;
};

struct RasterizerState
{
	ID3D11RasterizerState*		RS = nullptr;
	D3D11_FILL_MODE				fillMode = D3D11_FILL_SOLID;
	D3D11_CULL_MODE				cullMode = D3D11_CULL_BACK;
	bool						frontCounterClockwise = true;
	bool						depthBias = false;
	float						depthBiasClamp = 0.0f;
	float						slopeScaledDepthBias = 0.0f;
	bool						depthClipEnable = true;
	bool						scissorEnable = false;
	bool						multiSampleEnable = false;
	bool						antialiasedLineEnable = false;
};

// Helps with many repetitive tasks such as creating resources.
class Direct3D11
{
public:
	Direct3D11() {};
	~Direct3D11() {};

	bool Start(HWND hWnd, unsigned backbufferWidth, unsigned backbufferHeight);
	void Shutdown(void);
	bool Resize(unsigned width, unsigned height);

	ID3D11Device* GetDevice() { return _d3dDevice; }
	ID3D11DeviceContext* GetDeviceContext() { return _d3dDeviceContext; }
	IDXGISwapChain* GetSwapChain() { return _SwapChain; }
	ID3D11RenderTargetView* GetBackBufferRTV() { return _BackBufferRTV; }

	RenderTarget CreateRenderTarget(
		DXGI_FORMAT format,
		unsigned width,
		unsigned height,
		unsigned depth,
		unsigned flags = 0,
		unsigned arraySize = 1,
		unsigned mipLevels = 1,
		unsigned sampleCount = 1
		);

	void DeleteRenderTarget(RenderTarget &rt);

	DepthBuffer CreateDepthBuffer(
		DXGI_FORMAT format,
		unsigned width,
		unsigned height,
		bool sampleInShader = false,
		unsigned flags = 0,
		unsigned arraySize = 1,
		unsigned mipLevels = 1,
		unsigned sampleCount = 1
		);
	void DeleteDepthBuffer(DepthBuffer &db);

	StructuredBuffer CreateStructuredBuffer(
		unsigned stride,
		unsigned elementCount,
		bool CPUWrite = true,
		bool GPUWrite = false,
		const void *initialData = nullptr,
		bool addStructureCounter = false
		);
	StructuredBuffer CreateAppendConsumeBuffer(
		unsigned stride,
		unsigned elementCount,
		const void *initialData = nullptr
		);
	void DeleteStructuredBuffer(StructuredBuffer &sb);

	IndirectArgsBuffer CreateIndirectArgsBuffer(
		unsigned size,
		void *initialData = nullptr
		);
	void DeleteIndirectArgsBuffer(IndirectArgsBuffer &iab);


	const DepthStencilState CreateDepthStencilState(
		bool						depthEnable = true,
		D3D11_DEPTH_WRITE_MASK		depthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
		D3D11_COMPARISON_FUNC		depthComparison = D3D11_COMPARISON_LESS,
		bool						stencilEnable = false,
		UINT8						stencilReadMask = 0xFF,
		UINT8						stencilWriteMask = 0xFF)const;

	const void DeleteDepthStencilState(DepthStencilState& dss)const;

	const RasterizerState CreateRasterizerState(
		D3D11_FILL_MODE				fillMode = D3D11_FILL_SOLID,
		D3D11_CULL_MODE				cullMode = D3D11_CULL_BACK,
		bool						frontCounterClockwise = true,
		bool						depthBias = false,
		float						depthBiasClamp = 0.0f,
		float						slopeScaledDepthBias = 0.0f,
		bool						depthClipEnable = true,
		bool						scissorEnable = false,
		bool						multiSampleEnable = false,
		bool						antialiasedLineEnable = false)const;

	const void DeleteRasterizerState(RasterizerState& rs)const;

private:
	Direct3D11( const Direct3D11 &other );
	Direct3D11& operator=( const Direct3D11 &rhs );

	// Creates a view to a resource. Using DXGI_FORMAT_UNKNOWN uses the format
	// that the resource was created with. Specifying -1 for firstSlice creates
	// a view to the entire resource.
	ID3D11RenderTargetView* _CreateRTV(
		ID3D11Resource *resource,
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
		int firstSlice = -1,
		unsigned numSlices = 1
		);
	ID3D11ShaderResourceView* _CreateSRV(
		ID3D11Resource *resource,
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
		int firstSlice = -1,
		unsigned numSlices = 1
		);
	ID3D11UnorderedAccessView* _CreateUAV(
		ID3D11Resource *resource,
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
		int firstSlice = -1,
		unsigned numSlices = 1
		);
	ID3D11DepthStencilView* _CreateDSV(
		ID3D11Resource *resource,
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
		bool readOnly = false,
		int firstSlice = -1,
		unsigned numSlices = 1
		);
	ID3D11DepthStencilState* _CreateDSS(
		bool						depthEnable = true,
		D3D11_DEPTH_WRITE_MASK		depthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
		D3D11_COMPARISON_FUNC		depthComparison = D3D11_COMPARISON_LESS,
		bool						stencilEnable = false,
		UINT8						stencilReadMask = 0xFF,
		UINT8						stencilWriteMask = 0xFF
		)const;
	ID3D11RasterizerState* _CreateRS(
		D3D11_FILL_MODE				fillMode = D3D11_FILL_SOLID,
		D3D11_CULL_MODE				cullMode = D3D11_CULL_BACK,
		bool						frontCounterClockwise = true,
		bool						depthBias = false,
		float						depthBiasClamp = 0.0f,
		float						slopeScaledDepthBias = 0.0f,
		bool						depthClipEnable = true,
		bool						scissorEnable = false,
		bool						multiSampleEnable = false,
		bool						antialiasedLineEnable = false
		)const;
		//TODO: Create a depth stencil state, so I can disable writing to the depth buffer.
private:
	HWND _hWnd;

	ID3D11Device *_d3dDevice = nullptr;
	ID3D11DeviceContext *_d3dDeviceContext = nullptr;

	D3D_FEATURE_LEVEL _FeatureLevel;
	IDXGISwapChain *_SwapChain = nullptr;
	IDXGIOutput *_DXGIOutput = nullptr;
	ID3D11RenderTargetView *_BackBufferRTV = nullptr;
};

#endif