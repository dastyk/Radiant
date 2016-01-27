#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

//////////////
// Includes //
//////////////
#include <d3d11.h>
#include <vector>

#include <DirectXMath.h>

////////////////////
// Local Includes //
////////////////////
#include "Direct3D11.h"
#include "IRenderProvider.h"
#include "Mesh.h"
#include "Utils.h"
#include "Shader.h"

using namespace std;


class Graphics
{
public:
	Graphics();
	~Graphics();

	const void Init();
	const void Shutdown();

	void Render( double totalTime, double deltaTime );
	const void ResizeSwapChain()const;

	void AddRenderProvider( IRenderProvider *provider );

	bool CreateBuffers( Mesh *mesh, std::uint32_t& vertexBufferIndex, std::uint32_t& indexBufferIndex );

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


private:
	Direct3D11 *_D3D11 = nullptr;

	std::vector<IRenderProvider*> _RenderProviders;

	// Elements are submitted by render providers, and is cleared on every
	// frame. It's a member variable to avoid reallocating memory every frame.
	std::vector<RenderJob> _Meshes;

	std::vector<ID3D11Buffer*> _VertexBuffers;
	std::vector<ID3D11Buffer*> _IndexBuffers;
	std::vector<ID3D11VertexShader*> _VertexShaders;
	std::vector<ID3D11InputLayout*> _inputLayouts;
	std::vector<ID3D11PixelShader*> _pixelShaders;
};

#endif