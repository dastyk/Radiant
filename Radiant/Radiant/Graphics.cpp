#include "Graphics.h"

#include <d3dcompiler.h>
#include <DirectXMath.h> // for now
#include "System.h"

using namespace std;
using namespace DirectX;

Graphics::Graphics()
{
	_D3D11 = new Direct3D11();
}


Graphics::~Graphics()
{
	_D3D11->Shutdown();
	delete _D3D11;
}

void Graphics::Render( double totalTime, double deltaTime )
{
	ID3D11DeviceContext *deviceContext = _D3D11->GetDeviceContext();

	BeginFrame();

	// Clear depth stencil view
	//deviceContext->ClearDepthStencilView( _mainDepth.DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

	deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	_Meshes.clear();
	for ( auto renderProvider : _RenderProviders )
	{
		// TODO: Maybe the renderer should have methods that return a lambda containing
		// code that adds something. Instead of using craploads of providers, they could
		// get this function and call it whenever they want to add something?
		// Like get function, save for later, when renderer gathers they use their particular
		// function.
		renderProvider->GatherJobs( [this]( RenderJob& mesh ) -> /*const Material**/void
		{
			//Material *ret = nullptr;

			// If the material has not been set, we use a default material.
			// We also return a pointer to the default material so that the
			// original mesh material can use it.
			//if ( mesh.Material._ShaderIndex == -1 )
			//{
			//	mesh.Material = _NullMaterial;
			//	ret = &_NullMaterial;
			//}

			_Meshes.push_back( move( mesh ) );

			//return ret;
		} );
	}

	EndFrame();
}

HRESULT Graphics::OnCreateDevice( void )
{
	return S_OK;
}


void Graphics::OnDestroyDevice( void )
{

}


HRESULT Graphics::OnResizedSwapChain( void )
{
	return S_OK;
}


void Graphics::OnReleasingSwapChain( void )
{
	//_D3D11->DeleteDepthBuffer( _mainDepth );
}

void Graphics::AddRenderProvider( IRenderProvider *provider )
{
	_RenderProviders.push_back( provider );
}

void Graphics::BeginFrame(void)
{
	ID3D11DeviceContext* deviceContext = _D3D11->GetDeviceContext();
	if ( !deviceContext )
		return;

	static float clearColor[4] = { 0.0f, 0.0f, 0.25f, 1.0f };

	deviceContext->ClearRenderTargetView( _D3D11->GetBackBufferRTV(), clearColor );
}

void Graphics::EndFrame(void)
{
	//_swapChain->Present( _vSync ? 1 : 0, 0 );
	_D3D11->GetSwapChain()->Present( 1, 0 );
}

const void Graphics::Init()
{
	WindowHandler* h = System::GetInstance()->GetWindowHandler();
	if ( !_D3D11->Start( h->GetHWnd(), h->GetWindowWidth(), h->GetWindowHeight() ) )
		throw "Failed to initialize Direct3D11";

	if ( FAILED( OnCreateDevice() ) )
		throw "Failed to create device";
	if ( FAILED( OnResizedSwapChain() ) )
		throw "Failed to resize swap chain";

	return void();
}

const void Graphics::Shutdown()
{
	OnReleasingSwapChain();
	OnDestroyDevice();

	return void();
}
