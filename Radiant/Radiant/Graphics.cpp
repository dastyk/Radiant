#include "Graphics.h"

#include <d3dcompiler.h>
#include <DirectXMath.h> // for now
#include "System.h"
#include "Shader.h"

using namespace std;
using namespace DirectX;

Graphics::Graphics()
{

}


Graphics::~Graphics()
{

}

void Graphics::Render( double totalTime, double deltaTime )
{
	ID3D11DeviceContext *deviceContext = _D3D11->GetDeviceContext();

	BeginFrame();

	// Clear depth stencil view
	deviceContext->ClearDepthStencilView( _mainDepth.DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

	deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	_Meshes.clear();
	RenderJobMap jobs;
	for ( auto renderProvider : _RenderProviders )
	{
		// TODO: Maybe the renderer should have methods that return a lambda containing
		// code that adds something. Instead of using craploads of providers, they could
		// get this function and call it whenever they want to add something?
		// Like get function, save for later, when renderer gathers they use their particular
		// function.
		//renderProvider->GatherJobs( [this]( RenderJob& mesh ) -> /*const Material**/void
		//{
		//	//Material *ret = nullptr;

		//	// If the material has not been set, we use a default material.
		//	// We also return a pointer to the default material so that the
		//	// original mesh material can use it.
		//	//if ( mesh.Material._ShaderIndex == -1 )
		//	//{
		//	//	mesh.Material = _NullMaterial;
		//	//	ret = &_NullMaterial;
		//	//}

		//	_Meshes.push_back( move( mesh ) );

		//	//return ret;
		//} );

		renderProvider->GatherJobs(jobs);

	}
	/*std::vector<CamData&> data;
	for (auto camProvider : _cameraProviders)
	{
		camProvider->GatherCam([this](CamData& dat) -> void
		{
			data.push_bak()
		});
	}*/
	CamData cam;
	for (auto camProvider : _cameraProviders)
	{
		camProvider->GatherCam(cam);
	}
	_GBuffer->Clear( deviceContext );

	// Enable depth testing when rendering scene.
	ID3D11RenderTargetView *rtvs[] = { _GBuffer->ColorRT(), _GBuffer->NormalRT() };
	deviceContext->OMSetRenderTargets( 2, rtvs, _mainDepth.DSV );

	// Render the scene
	{
		deviceContext->IASetInputLayout( _inputLayout );

		XMMATRIX world, worldView, wvp, worldViewInvTrp, view, viewproj;

		for (auto& vB : jobs)
		{	
			uint32_t stride = sizeof(VertexLayout);
			uint32_t offset = 0;
			deviceContext->IASetVertexBuffers( 0, 1, &_VertexBuffers[vB.first], &stride, &offset );

			for (auto& iB : vB.second)
			{
				deviceContext->IASetIndexBuffer( _IndexBuffers[iB.first], DXGI_FORMAT_R32_UINT, 0 );

				for (auto& t : iB.second)
				{
					world = XMLoadFloat4x4( (XMFLOAT4X4*)t.first );
					view = XMLoadFloat4x4(&cam.viewMatrix);
					viewproj = XMLoadFloat4x4(&cam.viewProjectionMatrix);
					worldView = world *view;//  _cameraView;//
					// Don't forget to transpose matrices that go to the shader. This was
					// handled behind the scenes in effects framework. The reason for this
					// is that HLSL uses column major matrices whereas DirectXMath uses row
					// major. If one forgets to transpose matrices, when HLSL attempts to
					// read a column it's really a row.
					wvp = XMMatrixTranspose(world * viewproj);//_cameraView * _cameraProj );//
					worldViewInvTrp = XMMatrixInverse( nullptr, worldView ); // Normally transposed, but since it's done again for shader I just skip it

					// Set object specific constants.
					StaticMeshVSConstants vsConstants;
					XMStoreFloat4x4( &vsConstants.WVP, wvp );
					XMStoreFloat4x4( &vsConstants.WorldViewInvTrp, worldViewInvTrp );

					// Update shader constants.
					D3D11_MAPPED_SUBRESOURCE mappedData;
					deviceContext->Map( _staticMeshVSConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData );
					memcpy( mappedData.pData, &vsConstants, sizeof( StaticMeshVSConstants ) );
					deviceContext->Unmap( _staticMeshVSConstants, 0 );

					deviceContext->VSSetShader( _staticMeshVS, nullptr, 0 );
					deviceContext->VSSetConstantBuffers( 0, 1, &_staticMeshVSConstants );
					deviceContext->PSSetShader( _GBufferPS, nullptr, 0 );

					for (RenderJobMap4::iterator it = t.second.begin(); it != t.second.end(); it++)
					{
						deviceContext->DrawIndexed( it->IndexCount, it->IndexStart, 0 );
					}
				}
			}
		}
	}

	D3D11_VIEWPORT fullViewport;
	uint32_t numViewports = 1;
	deviceContext->RSGetViewports( &numViewports, &fullViewport );

	auto backbuffer = _D3D11->GetBackBufferRTV();
	deviceContext->OMSetRenderTargets( 1, &backbuffer, nullptr );

	//
	// Full-screen textured quad
	//

	// Specify 1 or 4.
	uint32_t numImages = 0;

	if ( GetAsyncKeyState( VK_NUMPAD1 ) & 0x8000 )
		numImages = 1;
	else if ( GetAsyncKeyState( VK_NUMPAD4 ) & 0x8000 )
		numImages = 4;

	if ( numImages )
	{
		// The first code is just to easily display 1 full screen image or
		// 4 smaller in quadrants. Simply select what resource views to use
		// and how many of those to draw.
		ID3D11ShaderResourceView *srvs[4] =
		{
			_GBuffer->ColorSRV(),
			_GBuffer->NormalSRV(),
			_GBuffer->ColorSRV(),
			_GBuffer->NormalSRV()
		};

		auto window = System::GetInstance()->GetWindowHandler();

		D3D11_VIEWPORT vp[4];
		for ( int i = 0; i < 4; ++i )
		{
			vp[i].MinDepth = 0.0f;
			vp[i].MaxDepth = 1.0f;
			vp[i].Width = window->GetWindowWidth() / 2.0f;
			vp[i].Height = window->GetWindowHeight() / 2.0f;
			vp[i].TopLeftX = (i % 2) * window->GetWindowWidth() / 2.0f;
			vp[i].TopLeftY = (uint32_t)(0.5f * i) * window->GetWindowHeight() / 2.0f;
		}

		if ( numImages == 1 )
		{
			vp[0].Width = static_cast<float>(window->GetWindowWidth());
			vp[0].Height = static_cast<float>(window->GetWindowHeight());
		}

		// Here begins actual render code

		for ( uint32_t i = 0; i < numImages; ++i )
		{
			ID3D11PixelShader *ps = _fullscreenTexturePSMultiChannel;
			if ( srvs[i] == _mainDepth.SRV )
				ps = _fullscreenTexturePSSingleChannel;

			deviceContext->RSSetViewports( 1, &vp[i] );
			deviceContext->VSSetShader( _fullscreenTextureVS, nullptr, 0 );
			deviceContext->PSSetShader( ps, nullptr, 0 );
			deviceContext->PSSetShaderResources( 0, 1, &srvs[i] );

			deviceContext->Draw( 3, 0 );

			ID3D11ShaderResourceView *nullSRV = nullptr;
			deviceContext->PSSetShaderResources( 0, 1, &nullSRV );
		}

		deviceContext->RSSetViewports( 1, &fullViewport );
	}
	
	EndFrame();
}

const void Graphics::ResizeSwapChain() const
{
	WindowHandler* w = System::GetWindowHandler();
	_D3D11->Resize(w->GetWindowWidth(), w->GetWindowHeight());
	return void();
}


HRESULT Graphics::OnCreateDevice( void )
{
	auto device = _D3D11->GetDevice();

	_staticMeshVS = CompileVSFromFile( device, L"Shaders/StaticMeshVS.hlsl", "VS", "vs_4_0", nullptr, nullptr, &_basicShaderInput );
	if ( !_staticMeshVS )
		return E_FAIL;

	if ( !_BuildInputLayout() )
		return E_FAIL;

	_GBufferPS = CompilePSFromFile( device, L"Shaders/GBuffer.hlsl", "PS", "ps_4_0" );
	if ( !_GBufferPS )
		return E_FAIL;

	_fullscreenTextureVS = CompileVSFromFile( device, L"Shaders/FullscreenTexture.hlsl", "VS", "vs_4_0" );
	_fullscreenTexturePSMultiChannel = CompilePSFromFile( device, L"Shaders/FullscreenTexture.hlsl", "PSMultiChannel", "ps_4_0" );
	_fullscreenTexturePSSingleChannel = CompilePSFromFile( device, L"Shaders/FullscreenTexture.hlsl", "PSSingleChannel", "ps_4_0" );
	if ( !_fullscreenTextureVS || !_fullscreenTexturePSMultiChannel || !_fullscreenTexturePSSingleChannel )
		return E_FAIL;

	D3D11_BUFFER_DESC bufDesc;
	memset( &bufDesc, 0, sizeof( D3D11_BUFFER_DESC ) );
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufDesc.ByteWidth = sizeof( Graphics::StaticMeshVSConstants );
	HR_RETURN( device->CreateBuffer( &bufDesc, nullptr, &_staticMeshVSConstants ) );

	return S_OK;
}


void Graphics::OnDestroyDevice( void )
{
	for (auto s : _pixelShaders)
		SAFE_RELEASE(s);
	for (auto s : _inputLayouts)
		SAFE_RELEASE(s);
	for (auto s : _VertexShaders)
		SAFE_RELEASE(s);

	SAFE_RELEASE( _inputLayout );
	SAFE_RELEASE( _staticMeshVS );
	SAFE_RELEASE( _staticMeshVSConstants );
	SAFE_RELEASE( _basicShaderInput );
	SAFE_RELEASE( _GBufferPS );

	SAFE_RELEASE( _fullscreenTextureVS );
	SAFE_RELEASE( _fullscreenTexturePSMultiChannel );
	SAFE_RELEASE( _fullscreenTexturePSSingleChannel );

	for ( ID3D11Buffer *b : _VertexBuffers )
	{
		SAFE_RELEASE( b );
	}

	for ( ID3D11Buffer *b : _IndexBuffers )
	{
		SAFE_RELEASE( b );
	}
}


HRESULT Graphics::OnResizedSwapChain( void )
{
	auto device = _D3D11->GetDevice();
	auto window = System::GetInstance()->GetWindowHandler();

	_mainDepth = _D3D11->CreateDepthBuffer( DXGI_FORMAT_D24_UNORM_S8_UINT, window->GetWindowWidth(), window->GetWindowHeight(), true );

	_GBuffer = new GBuffer( device, window->GetWindowWidth(), window->GetWindowHeight() );

	return S_OK;
}


void Graphics::OnReleasingSwapChain( void )
{
	_D3D11->DeleteDepthBuffer( _mainDepth );

	SAFE_DELETE( _GBuffer );
}

bool Graphics::CreateBuffers( Mesh *mesh, uint32_t& vertexBufferIndex, uint32_t& indexBufferIndex )
{
	void *vertexData = nullptr;
	uint32_t vertexDataSize = 0;
	void *indexData = nullptr;
	uint32_t indexDataSize = 0;
	_InterleaveVertexData( mesh, &vertexData, vertexDataSize, &indexData, indexDataSize );

	ID3D11Buffer *vertexBuffer = _CreateVertexBuffer( vertexData, vertexDataSize );
	ID3D11Buffer *indexBuffer = _CreateIndexBuffer( indexData, indexDataSize );
	if ( !vertexBuffer || !indexBuffer )
	{
		SAFE_DELETE_ARRAY( vertexData );
		SAFE_DELETE_ARRAY( indexData );
		SAFE_RELEASE( vertexBuffer );
		SAFE_RELEASE( indexBuffer );
		return false;
	}
	SAFE_DELETE_ARRAY( vertexData );
	SAFE_DELETE_ARRAY( indexData );

	_VertexBuffers.push_back( vertexBuffer );
	vertexBufferIndex = static_cast<unsigned int>(_VertexBuffers.size() - 1);
	_IndexBuffers.push_back( indexBuffer );
	indexBufferIndex = static_cast<unsigned int>(_IndexBuffers.size() - 1);

	return true;
}

ID3D11Buffer* Graphics::_CreateVertexBuffer( void *vertexData, std::uint32_t vertexDataSize )
{
	D3D11_BUFFER_DESC bufDesc;
	bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.ByteWidth = vertexDataSize;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = vertexData;

	ID3D11Buffer *buf = nullptr;
	if ( FAILED( _D3D11->GetDevice()->CreateBuffer( &bufDesc, &initData, &buf ) ) )
		return nullptr;

	return buf;
}

ID3D11Buffer* Graphics::_CreateIndexBuffer( void *indexData, std::uint32_t indexDataSize )
{
	D3D11_BUFFER_DESC bufDesc;
	bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufDesc.ByteWidth = indexDataSize;
	bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = indexData;

	ID3D11Buffer *buf = nullptr;
	if ( FAILED( _D3D11->GetDevice()->CreateBuffer( &bufDesc, &initData, &buf ) ) )
		return nullptr;

	return buf;
}

void Graphics::_InterleaveVertexData( Mesh *mesh, void **vertexData, std::uint32_t& vertexDataSize, void **indexData, std::uint32_t& indexDataSize )
{
	
	if ( vertexData )
	{
		VertexLayout *completeVertices = new VertexLayout[mesh->IndexCount()];

		auto positions = mesh->AttributeData( mesh->FindStream( Mesh::AttributeType::Position ) );
		auto positionIndices = mesh->AttributeIndices( mesh->FindStream( Mesh::AttributeType::Position ) );
		
		auto normals = mesh->AttributeData( mesh->FindStream( Mesh::AttributeType::Normal ) );
		auto normalIndices = mesh->AttributeIndices( mesh->FindStream( Mesh::AttributeType::Normal ) );
		
		auto tangents = mesh->AttributeData(mesh->FindStream(Mesh::AttributeType::Tangent));
		auto tangentIndices = mesh->AttributeIndices(mesh->FindStream(Mesh::AttributeType::Tangent));
		
		auto binormals = mesh->AttributeData(mesh->FindStream(Mesh::AttributeType::Binormal));
		auto binormalIndices = mesh->AttributeIndices(mesh->FindStream(Mesh::AttributeType::Binormal));
		
		auto texcoords = mesh->AttributeData(mesh->FindStream(Mesh::AttributeType::TexCoord));
		auto texcoordIndices = mesh->AttributeIndices(mesh->FindStream(Mesh::AttributeType::TexCoord));

		for ( unsigned i = 0; i < mesh->IndexCount(); ++i )
		{
			completeVertices[i]._position = ((XMFLOAT3*)positions.data())[positionIndices[i]];
			completeVertices[i]._normal = ((XMFLOAT3*)normals.data())[normalIndices[i]];
			completeVertices[i]._tangent = ((XMFLOAT3*)tangents.data())[tangentIndices[i]];
			completeVertices[i]._binormal = ((XMFLOAT3*)binormals.data())[binormalIndices[i]];
			completeVertices[i]._texCoords = ((XMFLOAT2*)texcoords.data())[texcoordIndices[i]];
		}

		*vertexData = completeVertices;
		vertexDataSize = sizeof( VertexLayout ) * mesh->IndexCount();
	}

	// TODO: mesh could have a function that returns an index buffer matched
	// for an arbitrary combination of attributes.
	if ( indexData )
	{
		unsigned *completeIndices = new unsigned[mesh->IndexCount()];
		unsigned counter = 0;
		for ( unsigned batch = 0; batch < mesh->BatchCount(); ++batch )
		{
			for ( unsigned i = 0; i < mesh->Batches()[batch].IndexCount; ++i )
			{
				completeIndices[counter++] = mesh->Batches()[batch].StartIndex + i;
			}
		}

		for ( unsigned i = 0; i < mesh->IndexCount(); i += 3 )
		{
			swap( completeIndices[i], completeIndices[i + 2] );
		}

		*indexData = completeIndices;
		indexDataSize = sizeof( unsigned ) * mesh->IndexCount();
	}
}

bool Graphics::_BuildInputLayout( void )
{
	//D3D11_APPEND_ALIGNED_ELEMENT kan användas på AlignedByteOffset för att lägga elementen direkt efter föregående

	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// Create the input layout.
	if ( FAILED( _D3D11->GetDevice()->CreateInputLayout( vertexDesc, 5, _basicShaderInput->GetBufferPointer(), _basicShaderInput->GetBufferSize(), &_inputLayout ) ) )
		return false;

	return true;
}

void Graphics::AddRenderProvider( IRenderProvider *provider )
{
	_RenderProviders.push_back( provider );
}

void Graphics::AddCameraProvider(ICameraProvider * provider)
{
	_cameraProviders.push_back(provider);
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
	_D3D11->GetSwapChain()->Present( System::GetOptions()->GetVsync() ? 1 : 0, 0 );
}


const void Graphics::Init()
{
	_D3D11 = new Direct3D11();
	WindowHandler* h = System::GetWindowHandler();
	if ( !_D3D11->Start( h->GetHWnd(), h->GetWindowWidth(), h->GetWindowHeight() ) )
		throw "Failed to initialize Direct3D11";

	if ( FAILED( OnCreateDevice() ) )
		throw "Failed to create device";
	if ( FAILED( OnResizedSwapChain() ) )
		throw "Failed to resize swap chain";

	_cameraView = XMMatrixLookAtLH( XMVectorSet( 0, 0, -50, 1 ), XMVectorSet( 0, 0, 0, 1 ), XMVectorSet( 0, 1, 0, 0 ) );
	_cameraProj = XMMatrixPerspectiveFovLH( 0.25f * XM_PI, 800.0f / 600.0f, 0.1f, 1000.0f );

	return void();
}

const void Graphics::Shutdown()
{
	OnReleasingSwapChain();
	OnDestroyDevice();
	_D3D11->Shutdown();
	delete _D3D11;
	return void();
}
