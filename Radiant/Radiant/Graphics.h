#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

//////////////
// Includes //
//////////////
#include <d3d11.h>

////////////////////
// Local Includes //
////////////////////
#include "General.h"
#include "Direct3D11.h"

#pragma comment (lib, "d3d11.lib")

class Graphics
{
public:
	Graphics();
	~Graphics();

	const void Init();
	const void Shutdown();

	void Render( double totalTime, double deltaTime );

private:

	HRESULT OnCreateDevice( void );
	void OnDestroyDevice( void );
	HRESULT OnResizedSwapChain( void );
	void OnReleasingSwapChain( void );

	void BeginFrame( void );
	void EndFrame( void );

private:
	Direct3D11 *_D3D11 = nullptr;

	std::uint32_t _width;
	std::uint32_t _height;
};

#endif