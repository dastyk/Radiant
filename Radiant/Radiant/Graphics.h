#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

//////////////
// Includes //
//////////////
#include <d3d11.h>
#include <vector>

////////////////////
// Local Includes //
////////////////////
#include "Direct3D11.h"
#include "IRenderProvider.h"

#pragma comment (lib, "d3d11.lib")

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

private:

	HRESULT OnCreateDevice( void );
	void OnDestroyDevice( void );
	HRESULT OnResizedSwapChain( void );
	void OnReleasingSwapChain( void );

	void BeginFrame( void );
	void EndFrame( void );


private:
	Direct3D11 *_D3D11 = nullptr;

	std::vector<IRenderProvider*> _RenderProviders;

	// Elements are submitted by render providers, and is cleared on every
	// frame. It's a member variable to avoid reallocating memory every frame.
	std::vector<RenderJob> _Meshes;
};

#endif