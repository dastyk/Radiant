#include "Graphics.h"

#include <d3dcompiler.h>
#include <DirectXMath.h> // for now
#include "System.h"

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
	//deviceContext->ClearDepthStencilView( _mainDepth.DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

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
	// All jobs gathered, you can now render

	for (auto& vB : jobs)
	{	
		// Bind the vertex buffer to be used 
		//vB.first

		for (auto& iB : vB.second)
		{
			// Bind the index buffer to be used 
			//iB.first;

			for (auto& t : iB.second)
			{
				// Bind the transformation matrix
				XMFLOAT4X4 transform = *(XMFLOAT4X4*)t.first;
				for (RenderJobMap4::iterator it = t.second.begin(); it != t.second.end(); it++)
				{
					// Render.
					it->IndexStart;
					it->IndexCount;
	
				}
			}
			
			
			
		}
	}
	
	EndFrame();
}

const void Graphics::ResizeSwapChain() const
{
	Options* o = System::GetOptions();
	_D3D11->Resize(o->GetScreenResolutionWidth(), o->GetScreenResolutionHeight());
	return void();
}


HRESULT Graphics::OnCreateDevice( void )
{
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
	
	return S_OK;
}


void Graphics::OnReleasingSwapChain( void )
{
	//_D3D11->DeleteDepthBuffer( _mainDepth );
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


ID3D11InputLayout * Graphics::_CreateInputLayout(D3D11_INPUT_ELEMENT_DESC * vertexDesc, ID3D10Blob * pVertexShaderBuffer, int numElements)const
{
	HRESULT hr;
	ID3D11InputLayout* out;
	hr = _D3D11->GetDevice()->CreateInputLayout(
		vertexDesc,
		numElements,
		pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(),
		&out);

	if (FAILED(hr))
	{
		throw ErrorMsg(5000020, L"Could not create input layout.");
	}

	return out;
}


void Graphics::_InterleaveVertexData( Mesh *mesh, void **vertexData, std::uint32_t& vertexDataSize, void **indexData, std::uint32_t& indexDataSize )
{
	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT2 TexCoord;
		XMFLOAT3 Normal;
	};

	if ( vertexData )
	{
		Vertex *completeVertices = new Vertex[mesh->IndexCount()];

		auto positions = mesh->AttributeData( mesh->FindStream( Mesh::AttributeType::Position ) );
		auto positionIndices = mesh->AttributeIndices( mesh->FindStream( Mesh::AttributeType::Position ) );
		auto texcoords = mesh->AttributeData( mesh->FindStream( Mesh::AttributeType::TexCoord ) );
		auto texcoordIndices = mesh->AttributeIndices( mesh->FindStream( Mesh::AttributeType::TexCoord ) );
		auto normals = mesh->AttributeData( mesh->FindStream( Mesh::AttributeType::Normal ) );
		auto normalIndices = mesh->AttributeIndices( mesh->FindStream( Mesh::AttributeType::Normal ) );

		for ( unsigned i = 0; i < mesh->IndexCount(); ++i )
		{
			completeVertices[i].Position = ((XMFLOAT3*)positions.data())[positionIndices[i]];
			completeVertices[i].TexCoord = ((XMFLOAT2*)texcoords.data())[texcoordIndices[i]];
			completeVertices[i].Normal = ((XMFLOAT3*)normals.data())[normalIndices[i]];
		}

		*vertexData = completeVertices;
		vertexDataSize = sizeof( Vertex ) * mesh->IndexCount();
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
