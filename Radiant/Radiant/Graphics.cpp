#include "Graphics.h"

#include <d3dcompiler.h>
#include <DirectXMath.h> // for now
#include "System.h"

using namespace std;
using namespace DirectX;

Graphics::Graphics() : _width( 0 ), _height( 0 )
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
	BeginFrame();


	EndFrame();
}

const void Graphics::ResizeSwapChain() const
{
	WindowHandler* h = System::GetInstance()->GetWindowHandler();
	_D3D11->Resize(h->GetWindowWidth(), h->GetWindowHeight());
	return void();
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


void Graphics::BeginFrame(void)
{
	ID3D11DeviceContext* deviceContext = _D3D11->GetDeviceContext();
	if (!_D3D11->GetDeviceContext())
		return;

	static float clearColor[4] = { 0.0f, 0.0f, 0.25f, 1.0f };

	deviceContext->ClearRenderTargetView(_D3D11->GetBackBufferRTV(), clearColor);
}

void Graphics::EndFrame(void)
{
	//_swapChain->Present( _vSync ? 1 : 0, 0 );
	_D3D11->GetSwapChain()->Present(1, 0);
}


const void Graphics::Init()
{
	WindowHandler* h = System::GetInstance()->GetWindowHandler();
	if (!_D3D11->Start(h->GetHWnd(), h->GetWindowWidth(), h->GetWindowHeight()))
		throw "Failed to initialize Direct3D11";

	if (FAILED(OnCreateDevice()))
		throw "Failed to create device";
	if (FAILED(OnResizedSwapChain()))
		throw "Failed to resize swap chain";

	return void();
}

const void Graphics::Shutdown()
{
	OnReleasingSwapChain();
	OnDestroyDevice();

	return void();
}
