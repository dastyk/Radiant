#include "Graphics.h"

#include <d3dcompiler.h>
#include <DirectXMath.h> // for now
#include "System.h"
#include "Shader.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"

using namespace std;
using namespace DirectX;

Graphics::Graphics()
{

}


Graphics::~Graphics()
{

}

void Graphics::Render(double totalTime, double deltaTime)
{
	ID3D11DeviceContext *deviceContext = _D3D11->GetDeviceContext();

	BeginFrame();

	// Clear depth stencil view
	deviceContext->ClearDepthStencilView(_mainDepth.DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Gather all the data use for rendering

	ctimer.TimeStart("Gather");
	_GatherRenderData(); //TODO: Add support for multithreading the gathering of data.(so that each provider can give jobs at the same time)
	ctimer.TimeEnd("Gather");
	// Render all the meshes provided


	timer.TimeStart("Render");
	_RenderMeshes();
	timer.TimeEnd("Render");
	// Render lights using tiled deferred shading

	timer.TimeStart("Tiled deferred");
	_RenderLightsTiled( deviceContext, totalTime ); // Are we sure we are actually culling the lights correctly?  It still takes about 0.1 sec to render only 15 lights. When I did deferred I could render thousands of lights.
													// Or maby its the BRDF that takes a bit more time that phong, or maby we do some unnecessary stuff.
	timer.TimeEnd("Tiled deferred");

	timer.TimeStart("Thing");
	// Place the composited lit texture on the back buffer.  // TODO: Bind the backbuffer as the output texture for the compute shader instead. Prob save some time.
	{
		auto backbuffer = _D3D11->GetBackBufferRTV();
		deviceContext->OMSetRenderTargets( 1, &backbuffer, nullptr );
		ID3D11InputLayout* nullIL = nullptr;
		deviceContext->IASetInputLayout(nullIL);
		deviceContext->VSSetShader( _fullscreenTextureVS, nullptr, 0 );
		deviceContext->PSSetShader( _fullscreenTexturePSMultiChannel, nullptr, 0 );
		deviceContext->PSSetShaderResources( 0, 1, &_accumulateRT.SRV );
		deviceContext->PSSetSamplers( 0, 1, &_triLinearSam );

		deviceContext->Draw( 3, 0 );

		// Unbind texture
		ID3D11ShaderResourceView *nullSRV = nullptr;
		deviceContext->PSSetShaderResources( 0, 1, &nullSRV );
	}
	timer.TimeEnd("Thing");


	// Render all the overlayes
	timer.TimeStart("Overlays");
	_RenderOverlays();
	timer.TimeEnd("Overlays");

	// Render texts
	timer.TimeStart("Text");
	_RenderTexts();
	timer.TimeEnd("Text");

	// Render the GBuffers to the screen
	auto i = System::GetInput();
	if (i->IsKeyDown(VK_NUMPAD4))
		_RenderGBuffers(4U);
	else if (i->IsKeyDown(VK_NUMPAD1))
		_RenderGBuffers(1U);

	EndFrame();
	timer.GetTime();
	ctimer.GetTime();
}

const void Graphics::ResizeSwapChain()
{
	WindowHandler* w = System::GetWindowHandler();
	_D3D11->Resize(w->GetWindowWidth(), w->GetWindowHeight());
	OnResizedSwapChain();
	return void();
}


HRESULT Graphics::OnCreateDevice( void )
{
	auto device = _D3D11->GetDevice();

	_defaultMaterial = GenerateMaterial( L"Shaders/GBuffer.hlsl" );

	_staticMeshVS = CompileVSFromFile( device, L"Shaders/StaticMeshVS.hlsl", "VS", "vs_4_0", nullptr, nullptr, &_basicShaderInput );
	if ( !_staticMeshVS )
		return E_FAIL;

	if ( !_BuildInputLayout() )
		return E_FAIL;

	_tiledDeferredCS = CompileCSFromFile( device, L"Shaders/TiledDeferred.hlsl", "CS", "cs_5_0" );
	if ( !_tiledDeferredCS )
		return E_FAIL;

	_fullscreenTextureVS = CompileVSFromFile( device, L"Shaders/FullscreenTexture.hlsl", "VS", "vs_4_0" );
	_fullscreenTexturePSMultiChannel = CompilePSFromFile( device, L"Shaders/FullscreenTexture.hlsl", "PSMultiChannel", "ps_4_0" );
	_fullscreenTexturePSSingleChannel = CompilePSFromFile( device, L"Shaders/FullscreenTexture.hlsl", "PSSingleChannel", "ps_4_0" );
	_textVSShader = CompileVSFromFile(device, L"Shaders/TextVSShader.hlsl", "main", "vs_5_0", nullptr, nullptr, &_textShaderInput);
	_textPSShader = CompilePSFromFile(device, L"Shaders/TextPSShader.hlsl", "main", "ps_5_0");

	if ( !_fullscreenTextureVS || !_fullscreenTexturePSMultiChannel || !_fullscreenTexturePSSingleChannel || !_textVSShader || !_textPSShader)
		return E_FAIL;

	if (!_BuildTextInputLayout())
		return E_FAIL;

	D3D11_BUFFER_DESC bufDesc;
	memset( &bufDesc, 0, sizeof( D3D11_BUFFER_DESC ) );
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufDesc.ByteWidth = sizeof( Graphics::StaticMeshVSConstants );
	HR_RETURN( device->CreateBuffer( &bufDesc, nullptr, &_staticMeshVSConstants ) );

	bufDesc.ByteWidth = sizeof( Graphics::TiledDeferredConstants );
	HR_RETURN( device->CreateBuffer( &bufDesc, nullptr, &_tiledDeferredConstants ) );

	bufDesc.ByteWidth = sizeof(Graphics::TextVSConstants);
	HR_RETURN(device->CreateBuffer(&bufDesc, nullptr, &_textVSConstantBuffer));

	bufDesc.ByteWidth = sizeof(Graphics::TextPSConstants);
	HR_RETURN(device->CreateBuffer(&bufDesc, nullptr, &_textPSConstantBuffer));


	

	D3D11_SAMPLER_DESC samDesc;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samDesc.MaxAnisotropy = 16;
	samDesc.MinLOD = -FLT_MAX;
	samDesc.MaxLOD = FLT_MAX;
	samDesc.MipLODBias = 0.0f;
	HR_RETURN( device->CreateSamplerState( &samDesc, &_triLinearSam ) );

	_pointLightsBuffer = _D3D11->CreateStructuredBuffer( sizeof( PointLight ), 1024 );
	_spotLightsBuffer = _D3D11->CreateStructuredBuffer( sizeof( SpotLight ), 1024 );
	_capsuleLightsBuffer = _D3D11->CreateStructuredBuffer( sizeof( CapsuleLight ), 1024 );
	_areaRectLightBuffer = _D3D11->CreateStructuredBuffer(sizeof(AreaRectLight), 1024);

	return S_OK;
}


void Graphics::OnDestroyDevice( void )
{
	operator delete(_defaultMaterial.ConstantsMemory);
	_defaultMaterial.ConstantsMemorySize = 0;
	_defaultMaterial.Constants.clear();
	_defaultMaterial.Shader = -1;
	SAFE_DELETE( _defaultMaterial.Textures );
	_defaultMaterial.TextureCount = 0;
	_defaultMaterial.TextureOffsets.clear();

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

	SAFE_RELEASE( _tiledDeferredCS );
	SAFE_RELEASE( _tiledDeferredConstants );

	SAFE_RELEASE( _fullscreenTextureVS );
	SAFE_RELEASE( _fullscreenTexturePSMultiChannel );
	SAFE_RELEASE( _fullscreenTexturePSSingleChannel );

	SAFE_RELEASE(_textVSShader);
	SAFE_RELEASE(_textPSShader);
	SAFE_RELEASE(_textShaderInput);
	SAFE_RELEASE(_textInputLayot);
	SAFE_RELEASE(_textVSConstantBuffer);
	SAFE_RELEASE(_textPSConstantBuffer);


	_D3D11->DeleteStructuredBuffer( _pointLightsBuffer );
	_D3D11->DeleteStructuredBuffer(_spotLightsBuffer);
	_D3D11->DeleteStructuredBuffer( _spotLightsBuffer );
	_D3D11->DeleteStructuredBuffer( _capsuleLightsBuffer );
	_D3D11->DeleteStructuredBuffer(_areaRectLightBuffer);

	for ( auto srv : _textures )
	{
		SAFE_RELEASE( srv );
	}

	for ( ID3D11Buffer *b : _VertexBuffers )
	{
		SAFE_RELEASE( b );
	}

	for ( ID3D11Buffer *b : _IndexBuffers )
	{
		SAFE_RELEASE( b );
	}

	SAFE_RELEASE( _materialConstants );
	for ( auto s : _materialShaders )
		SAFE_RELEASE( s );

	SAFE_RELEASE( _triLinearSam );
}


HRESULT Graphics::OnResizedSwapChain( void )
{
	auto device = _D3D11->GetDevice();
	auto window = System::GetInstance()->GetWindowHandler();

	_D3D11->DeleteDepthBuffer(_mainDepth);
	_mainDepth = _D3D11->CreateDepthBuffer( DXGI_FORMAT_D24_UNORM_S8_UINT, window->GetWindowWidth(), window->GetWindowHeight(), true );
	_D3D11->DeleteRenderTarget(_accumulateRT);
	_accumulateRT = _D3D11->CreateRenderTarget( DXGI_FORMAT_R16G16B16A16_FLOAT, window->GetWindowWidth(), window->GetWindowHeight(), 0, TEXTURE_COMPUTE_WRITE );

	SAFE_DELETE(_GBuffer);
	_GBuffer = new GBuffer( device, window->GetWindowWidth(), window->GetWindowHeight() );


	DirectX::XMStoreFloat4x4(&_orthoMatrix, DirectX::XMMatrixOrthographicLH((float)window->GetWindowWidth(), (float)window->GetWindowHeight(), 0.001f, 10.0f));

	DirectX::XMStoreFloat4x4(&_orthoMatrix, DirectX::XMMatrixOrthographicLH((float)window->GetWindowWidth(), (float)window->GetWindowHeight(), 0.001f, 10.0f));
	
	return S_OK;
}


void Graphics::OnReleasingSwapChain( void )
{
	_D3D11->DeleteDepthBuffer( _mainDepth );

	_D3D11->DeleteRenderTarget( _accumulateRT );

	SAFE_DELETE( _GBuffer );
}

bool Graphics::CreateMeshBuffers( Mesh *mesh, uint32_t& vertexBufferIndex, uint32_t& indexBufferIndex )
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

uint Graphics::CreateTextBuffer(FontData & data)
{
	void *vertexData = nullptr;
	uint32_t vertexDataSize = 0;
	_BuildVertexData(data, (TextVertexLayout*&)vertexData, vertexDataSize);
	//vertexDataSize = 1024 * 6 * sizeof(TextVertexLayout);
	ID3D11Buffer *vertexBuffer = _CreateDynamicVertexBuffer(vertexData, 6*sizeof(TextVertexLayout)*32); // Buffer was to big I think thats why it crashed some times.
	if (!vertexBuffer)
	{
		SAFE_DELETE_ARRAY(vertexData);
		SAFE_RELEASE(vertexBuffer);
		throw ErrorMsg(5000036, L"Failed to create Text Buffer.");
	}
	SAFE_DELETE_ARRAY(vertexData);

	_VertexBuffers.push_back(vertexBuffer);
	return static_cast<unsigned int>(_VertexBuffers.size() - 1);
}

const void Graphics::UpdateTextBuffer(uint buffer, FontData & data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	void *vertexData = nullptr;
	uint32_t vertexDataSize = 0;
	_BuildVertexData(data, (TextVertexLayout*&)vertexData, vertexDataSize);

	auto deviceContext = _D3D11->GetDeviceContext();



	// Lock the vertex buffer so it can be written to.
	if(FAILED(deviceContext->Map(_VertexBuffers[ data.VertexBuffer], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		TraceDebug("Failed to map to text vertex buffer.");
		return;
	}

	// Copy the data into the vertex buffer.
	memcpy(mappedResource.pData, (void*)vertexData, vertexDataSize);

	// Unlock the vertex buffer.
	deviceContext->Unmap(_VertexBuffers[data.VertexBuffer], 0);

	SAFE_DELETE_ARRAY(vertexData);
	return void();
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

ID3D11Buffer * Graphics::_CreateDynamicVertexBuffer(void * vertexData, std::uint32_t vertexDataSize)
{
	D3D11_BUFFER_DESC bufDesc;
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.ByteWidth = vertexDataSize;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = vertexData;

	ID3D11Buffer *buf = nullptr;
	if (FAILED(_D3D11->GetDevice()->CreateBuffer(&bufDesc, &initData, &buf)))
		return nullptr;

	return buf;
}

const void Graphics::_BuildVertexData(FontData& data, TextVertexLayout*& vertexPtr, uint32_t& vertexDataSize)
{
	uint numLetters, index, i, letter;
	float drawX, drawY;
	drawX = DirectX::XMVectorGetX(DirectX::XMLoadFloat3(&data.pos));
	drawY = -DirectX::XMVectorGetY(DirectX::XMLoadFloat3(&data.pos));

	// Get the number of letters in the sentence.
	numLetters = (uint)data.text.size();
	vertexDataSize = sizeof(TextVertexLayout)* numLetters * 6;
	vertexPtr = new TextVertexLayout[numLetters * 6];

	// Initialize the index to the vertex array.
	index = 0;

	// Draw each letter onto a quad.
	for (i = 0; i < numLetters; i++)
	{
		letter = ((int)data.text[i]) - data.font->offset;

		// If the letter is a space then just move over three pixels.
		if (letter == 0)
		{
			drawX = drawX + (uint)((float)data.font->refSize*data.FontSize*0.4);
		}
		else
		{
			
			// First triangle in quad.
			vertexPtr[index]._position = XMFLOAT3(drawX, drawY - ((float)data.FontSize*(float)data.font->refSize), 1.0f);  // Bottom left
			vertexPtr[index]._texCoords = XMFLOAT2(data.font->Font[letter].left, 1.0f);
			index++;

			vertexPtr[index]._position = XMFLOAT3(drawX, drawY, 1.0f);  // Top left
			vertexPtr[index]._texCoords = XMFLOAT2(data.font->Font[letter].left, 0.0f);
			index++;

			vertexPtr[index]._position = XMFLOAT3(drawX + ((float)data.font->Font[letter].size*(float)data.FontSize), drawY, 1.0f);  // Top right.
			vertexPtr[index]._texCoords = XMFLOAT2(data.font->Font[letter].right, 0.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index]._position = XMFLOAT3((drawX + ((float)data.font->Font[letter].size)*(float)data.FontSize), (drawY - ((float)data.FontSize)*(float)data.font->refSize), 1.0f);  // Bottom right.
			vertexPtr[index]._texCoords = XMFLOAT2(data.font->Font[letter].right, 1.0f);
			index++;

			vertexPtr[index]._position = XMFLOAT3(drawX, drawY - ((float)data.FontSize*(float)data.font->refSize), 1.0f);  // bottom left.
			vertexPtr[index]._texCoords = XMFLOAT2(data.font->Font[letter].left, 1.0f);
			index++;

			vertexPtr[index]._position = XMFLOAT3(drawX + ((float)data.font->Font[letter].size*(float)data.FontSize), drawY, 1.0f);  // Top right.
			vertexPtr[index]._texCoords = XMFLOAT2(data.font->Font[letter].right, 0.0f);
			index++;

	

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + (uint)((data.font->Font[letter].size )*(float)data.FontSize + 1.0f);
		}
	}

	return;
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

// Constraints: The main function is called PS. The buffer containing constants is called Material.

// Takes a string of what shader file to compile. Allocates memory representing
// the layout expected by the shader. Returns this memory along with a map of
// where certain constants are located in the memory.
ShaderData Graphics::GenerateMaterial( const wchar_t *shaderFile )
{
	// Compile the shader.
	ID3D10Blob *byteCode = nullptr;
	auto materialShader = CompilePSFromFile( _D3D11->GetDevice(), shaderFile, "PS", "ps_4_0", nullptr, nullptr, &byteCode );

	// Reflect the compiled shader to extract information about what constants
	// exist and where.
	ID3D11ShaderReflection *refl = nullptr;
	if ( FAILED( D3DReflect( byteCode->GetBufferPointer(), byteCode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&refl ) ) )
	{
		SAFE_RELEASE( byteCode );
		SAFE_RELEASE( materialShader );
		TraceDebug( "Reflection of '%ls' failed", shaderFile );

		throw ErrorMsg(5000030, L"Reflection failed");
	}

	ShaderData material;
	material.Shader = (int32_t)_materialShaders.size();
	_materialShaders.push_back( materialShader );

	D3D11_SHADER_DESC reflDesc;
	refl->GetDesc( &reflDesc );

	ID3D11ShaderReflectionConstantBuffer *cb = refl->GetConstantBufferByName( "Material" );
	D3D11_SHADER_BUFFER_DESC cbDesc;
	cb->GetDesc( &cbDesc );
	
	// Allocate memory to store the constants.
	material.ConstantsMemory = operator new(cbDesc.Size);
	material.ConstantsMemorySize = cbDesc.Size;

	// Setup the constants in the material. We want to be able to access a constant
	// by name, and given this name know where its part of the memory is.
	for ( uint32_t i = 0; i < cbDesc.Variables; ++i )
	{
		ID3D11ShaderReflectionVariable *var = cb->GetVariableByIndex( i );
		D3D11_SHADER_VARIABLE_DESC varDesc;
		var->GetDesc( &varDesc );

		ShaderData::Constant constant;
		constant.Offset = varDesc.StartOffset;
		constant.Size = varDesc.Size;

		// Copy default value if we have one. Otherwise just zero the memory.
		if ( varDesc.DefaultValue )
			memcpy( (char*)material.ConstantsMemory + constant.Offset, varDesc.DefaultValue, varDesc.Size );
		else
			memset( (char*)material.ConstantsMemory + constant.Offset, 0, varDesc.Size );

		material.Constants[varDesc.Name] = constant;
	}

	_EnsureMinimumMaterialCBSize( cbDesc.Size );

	// Find out what texture bindings are present
	int32_t maxRegister = -1;
	for ( uint32_t i = 0; i < reflDesc.BoundResources; ++i )
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		refl->GetResourceBindingDesc( i, &bindDesc );
		
		// We are interested in textures.
		if ( bindDesc.Type == D3D_SIT_TEXTURE )
		{
			// Represents offset into the Textures array for this particular texture.
			// This is to map the correct texture name to the correct bind point.
			material.TextureOffsets[bindDesc.Name] = bindDesc.BindPoint;

			if ( static_cast<int32_t>(bindDesc.BindPoint) > maxRegister )
				maxRegister = bindDesc.BindPoint;
		}
	}
	
	// If we have textures, allocate an array to hold texture indices.
	if ( maxRegister >= 0 )
	{
		material.Textures = new int32_t[maxRegister + 1];

		for ( int32_t i = 0; i <= maxRegister; ++i )
			material.Textures[i] = -1;
	}

	material.TextureCount = maxRegister + 1;

	SAFE_RELEASE( byteCode );

	return material;
}

// Makes sure that the constant buffer holding material constants is sufficiently large.
void Graphics::_EnsureMinimumMaterialCBSize( uint32_t size )
{
	if ( size <= _currentMaterialCBSize )
		return;

	_currentMaterialCBSize = size;

	SAFE_RELEASE( _materialConstants );

	D3D11_BUFFER_DESC bufDesc;
	memset( &bufDesc, 0, sizeof( D3D11_BUFFER_DESC ) );
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.ByteWidth = size;
	_D3D11->GetDevice()->CreateBuffer( &bufDesc, nullptr, &_materialConstants );
}

const void Graphics::_GatherRenderData()
{
	// Gather meshes to render
	_renderJobs.clear();
	for (auto renderProvider : _RenderProviders)
		renderProvider->GatherJobs(_renderJobs);

	// Lights
	_pointLights.clear();
	_spotLights.clear();
	_capsuleLights.clear();
	_areaRectLights.clear();
	for ( auto lightProvider : _lightProviders )
		lightProvider->GatherLights(_pointLights, _spotLights, _capsuleLights, _areaRectLights);

	// Get the current camera
	for (auto camProvider : _cameraProviders)
		camProvider->GatherCam(_renderCamera);

	// Gather overlaydata
	_overlayRenderJobs.clear();
	for (auto overlayprovider : _overlayProviders)
	{
		overlayprovider->GatherOverlayJobs([this](OverlayData& data) -> void
		{
			_overlayRenderJobs.push_back(data);
		});
	}

	_textJobs.clear();

	for (auto textprovider : _textProviders)
		textprovider->GatherTextJobs(_textJobs);

	return void();
}

const void Graphics::_RenderMeshes()
{
	auto deviceContext = _D3D11->GetDeviceContext();

	_GBuffer->Clear(deviceContext);

	// Enable depth testing when rendering scene.
	ID3D11RenderTargetView *rtvs[] = { _GBuffer->ColorRT(), _GBuffer->NormalRT() };
	deviceContext->OMSetRenderTargets(2, rtvs, _mainDepth.DSV);

	// Render the scene
	{
		deviceContext->IASetInputLayout(_inputLayout);

		XMMATRIX world, worldView, wvp, worldViewInvTrp, view, viewproj;
		view = XMLoadFloat4x4(&_renderCamera.viewMatrix);
		viewproj = XMLoadFloat4x4(&_renderCamera.viewProjectionMatrix);
		deviceContext->VSSetShader(_staticMeshVS, nullptr, 0);
		deviceContext->PSSetShader(_materialShaders[_defaultMaterial.Shader], nullptr, 0);
		for (auto& vB : _renderJobs)
		{
			uint32_t stride = sizeof(VertexLayout);
			uint32_t offset = 0;
			deviceContext->IASetVertexBuffers(0, 1, &_VertexBuffers[vB.first], &stride, &offset);

			for (auto& iB : vB.second)
			{
				deviceContext->IASetIndexBuffer(_IndexBuffers[iB.first], DXGI_FORMAT_R32_UINT, 0);

				for (auto& t : iB.second)
				{
					world = XMLoadFloat4x4((XMFLOAT4X4*)t.first);


					worldView = world * view;
					// Don't forget to transpose matrices that go to the shader. This was
					// handled behind the scenes in effects framework. The reason for this
					// is that HLSL uses column major matrices whereas DirectXMath uses row
					// major. If one forgets to transpose matrices, when HLSL attempts to
					// read a column it's really a row.
					wvp = XMMatrixTranspose(world * viewproj);
					worldViewInvTrp = XMMatrixInverse(nullptr, worldView); // Normally transposed, but since it's done again for shader I just skip it

																		   // Set object specific constants.
					StaticMeshVSConstants vsConstants;
					XMStoreFloat4x4(&vsConstants.WVP, wvp);
					XMStoreFloat4x4(&vsConstants.WorldViewInvTrp, worldViewInvTrp);

					// Update shader constants.
					D3D11_MAPPED_SUBRESOURCE mappedData;
					deviceContext->Map(_staticMeshVSConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
					memcpy(mappedData.pData, &vsConstants, sizeof(StaticMeshVSConstants));
					deviceContext->Unmap(_staticMeshVSConstants, 0);

					for (RenderJobMap4::iterator it = t.second.begin(); it != t.second.end(); it++)
					{
						// TODO: Put the material in as the hash value for the job map, so that we only need to bind the material, and textures once per frame. Instead of once per mesh part.
						// Basiclly sorting after material aswell // if we define a max texture count in the shader, we can easily do an insertion sort.(like we have now)

						// TODO: Also make sure that we were given enough materials. If there is no material
						// for this mesh we can use a default one.
						//deviceContext->PSSetShader( _materialShaders[_defaultMaterial.Shader], nullptr, 0 );
						deviceContext->Map(_materialConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
						memcpy(mappedData.pData, (*it)->Material.ConstantsMemory, (*it)->Material.ConstantsMemorySize);
						deviceContext->Unmap(_materialConstants, 0);

						deviceContext->VSSetConstantBuffers(0, 1, &_staticMeshVSConstants);

						deviceContext->PSSetShader(_materialShaders[(*it)->Material.Shader], nullptr, 0);
						deviceContext->PSSetConstantBuffers(0, 1, &_materialConstants);
						deviceContext->PSSetSamplers(0, 1, &_triLinearSam);

						// Find the actual srvs to use.
						ID3D11ShaderResourceView **srvs = new ID3D11ShaderResourceView*[(*it)->Material.TextureCount];
						for (uint32_t i = 0; i < (*it)->Material.TextureCount; ++i)
						{
							int32_t textureIndex = (*it)->Material.Textures[i];
							if (textureIndex != -1)
							{
								srvs[i] = _textures[textureIndex];
							}
							else
							{
								srvs[i] = nullptr;
							}
						}

						deviceContext->PSSetShaderResources(0, (*it)->Material.TextureCount, srvs);

						SAFE_DELETE_ARRAY(srvs);

						deviceContext->DrawIndexed((*it)->IndexCount, (*it)->IndexStart, 0);
					}
				}
			}
		}
	}



	return void();
}

void Graphics::_RenderLightsTiled( ID3D11DeviceContext *deviceContext, double totalTime )
{
	// Unbind any potentially bound render targets
	deviceContext->OMSetRenderTargets( 0, nullptr, nullptr );

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	deviceContext->ClearUnorderedAccessViewFloat( _accumulateRT.UAV, clearColor );

	// Write light data to GPU buffer.
	D3D11_MAPPED_SUBRESOURCE mappedData;
	ID3D11Resource *resource;

	// Update point lights. Start by adding a null light that is used for clamping
	// in the compute shader. This is removed afterwards not to remain with the lights.
	PointLight nullPointLight;
	memset( &nullPointLight, 0, sizeof( PointLight ) );
	nullPointLight.range = -D3D11_FLOAT32_MAX; // Negative range to fail intersection test.
	_pointLights.push_back( &nullPointLight );

	_pointLightsBuffer.SRV->GetResource( &resource );
	deviceContext->Map( resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData );
	uint offset = 0;
	for (auto l : _pointLights)
	{
		((PointLight*)mappedData.pData)[offset] = *l;
		offset++;
	}
	deviceContext->Unmap( resource, 0 );
	SAFE_RELEASE( resource );

	_pointLights.pop_back(); // Remove temporary null light

	// Update spotlights
	SpotLight nullSpotLight;
	memset( &nullSpotLight, 0, sizeof( SpotLight ) );
	nullSpotLight.RangeRcp = -1e-6f; // Small negative (range large negative) to fail intersection.
	_spotLights.push_back( &nullSpotLight );

	_spotLightsBuffer.SRV->GetResource( &resource );
	deviceContext->Map( resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData );
	offset = 0;
	for (auto l : _spotLights)
	{
		((SpotLight*)mappedData.pData)[offset] = *l;
		offset++;
	}
	deviceContext->Unmap( resource, 0 );
	SAFE_RELEASE( resource );

	_spotLights.pop_back();

	// Update capsule lights
	CapsuleLight nullCapsuleLight;
	memset( &nullCapsuleLight, 0, sizeof( CapsuleLight ) );
	nullCapsuleLight.RangeRcp = -1e-6f; // Small negative (range large negative) to fail intersection.
	_capsuleLights.push_back( &nullCapsuleLight );

	_capsuleLightsBuffer.SRV->GetResource( &resource );
	deviceContext->Map( resource, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedData );
	offset = 0;
	for (auto l : _capsuleLights)
	{
		((CapsuleLight*)mappedData.pData)[offset] = *l;
		offset++;
	}
	deviceContext->Unmap( resource, 0 );
	SAFE_RELEASE( resource );

	_capsuleLights.pop_back();

	//Update arearectlights
	AreaRectLight nullAreaRectLight;
	memset(&nullAreaRectLight, 0, sizeof(AreaRectLight));
	nullAreaRectLight.Range = -D3D11_FLOAT32_MAX;
	_areaRectLights.push_back(&nullAreaRectLight);

	_areaRectLightBuffer.SRV->GetResource(&resource);
	deviceContext->Map(resource, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	offset = 0;
	for (auto l : _areaRectLights)
	{	
		((AreaRectLight*)mappedData.pData)[offset] = *l;
		offset++;
	}
	deviceContext->Unmap(resource, 0);
	SAFE_RELEASE(resource);
	_areaRectLights.pop_back();

	// Set shader constants (GBuffer, lights, matrices and so forth)
	TiledDeferredConstants constants;
	XMStoreFloat4x4( &constants.View, XMMatrixTranspose( XMLoadFloat4x4( &_renderCamera.viewMatrix ) ) );
	XMStoreFloat4x4( &constants.Proj, XMMatrixTranspose( XMLoadFloat4x4( &_renderCamera.projectionMatrix ) ) );
	XMStoreFloat4x4( &constants.InvView, XMMatrixTranspose( XMMatrixInverse( nullptr, XMLoadFloat4x4( &_renderCamera.viewMatrix ) ) ) );
	XMStoreFloat4x4( &constants.InvProj, XMMatrixTranspose( XMMatrixInverse( nullptr, XMLoadFloat4x4( &_renderCamera.projectionMatrix ) ) ) );
	WindowHandler* w = System::GetWindowHandler();
	constants.BackbufferWidth = static_cast<float>(w->GetWindowWidth());
	constants.BackbufferHeight = static_cast<float>(w->GetWindowHeight());
	constants.PointLightCount = min( _pointLights.size(), 1024 );
	constants.SpotLightCount = min( _spotLights.size(), 1024 );
	constants.CapsuleLightCount = min( _capsuleLights.size(), 1024 );
	constants.AreaRectLightCount = min(_areaRectLights.size(), 1024);

	deviceContext->Map( _tiledDeferredConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData );
	memcpy( mappedData.pData, &constants, sizeof( TiledDeferredConstants ) );
	deviceContext->Unmap( _tiledDeferredConstants, 0 );

	//Texture2D<float4> gColorMap : register(t0);
	//Texture2D<float4> gNormalMap : register(t1);
	//Texture2D<float4> gDepthMap : register(t2);
	//StructuredBuffer<PointLight> gPointLights : register(t4);
	ID3D11ShaderResourceView *srvs[] =
	{
		_GBuffer->ColorSRV(),
		_GBuffer->NormalSRV(),
		_mainDepth.SRV,
		nullptr,
		_pointLightsBuffer.SRV,
		_spotLightsBuffer.SRV,
		_capsuleLightsBuffer.SRV,
		_areaRectLightBuffer.SRV
	};

	ID3D11Buffer *buffers[] = { _tiledDeferredConstants };
	deviceContext->VSSetShader( nullptr, nullptr, 0 );
	deviceContext->PSSetShader( nullptr, nullptr, 0 );
	deviceContext->CSSetShader( _tiledDeferredCS, nullptr, 0 );
	deviceContext->CSSetConstantBuffers( 0, 1, buffers );
	deviceContext->CSSetSamplers( 0, 1, &_triLinearSam );
	deviceContext->CSSetShaderResources( 0, 8, srvs );
	deviceContext->CSSetUnorderedAccessViews( 0, 1, &_accumulateRT.UAV, nullptr );

	int groupCount[2];
	groupCount[0] = static_cast<uint32_t>(ceil( w->GetWindowWidth() / 16.0 ));
	groupCount[1] = static_cast<uint32_t>(ceil( w->GetWindowHeight() / 16.0 ));

	deviceContext->Dispatch( groupCount[0], groupCount[1], 1 );

	// Unbind stuff
	for ( int i = 0; i < 8; ++i )
	{
		srvs[i] = nullptr;
	}
	deviceContext->CSSetShaderResources( 0, 8, srvs );
	ID3D11UnorderedAccessView *nullUAV[] = { nullptr };
	deviceContext->CSSetUnorderedAccessViews( 0, 1, nullUAV, nullptr );
	deviceContext->CSSetShader( nullptr, nullptr, 0 );
	deviceContext->CSSetConstantBuffers( 0, 0, nullptr );
	deviceContext->CSSetSamplers( 0, 0, nullptr );
}

const void Graphics::_RenderOverlays() const
{
	auto deviceContext = _D3D11->GetDeviceContext();
	auto backbuffer = _D3D11->GetBackBufferRTV();
	deviceContext->OMSetRenderTargets(1, &backbuffer, nullptr);

	//Render all the overlays
	D3D11_VIEWPORT fullViewport;
	uint32_t numViewports = 1;
	deviceContext->RSGetViewports(&numViewports, &fullViewport);

	//Bind the shader
	ID3D11PixelShader *ps = _fullscreenTexturePSMultiChannel;
	deviceContext->VSSetShader(_fullscreenTextureVS, nullptr, 0);
	deviceContext->PSSetShader(ps, nullptr, 0);



	ID3D11ShaderResourceView *nullSRV = nullptr;
	deviceContext->PSSetShaderResources(0, 1, &nullSRV);

	for (auto& job : _overlayRenderJobs)
	{
		// Find the actual srvs to use.
		ID3D11ShaderResourceView **srvs = new ID3D11ShaderResourceView*[job.material.TextureCount];
		for (uint32_t i = 0; i < job.material.TextureCount; ++i)
		{
			int32_t textureIndex = job.material.Textures[i];
			if (textureIndex != -1)
			{
				srvs[i] = _textures[textureIndex];
			}
			else
			{
				srvs[i] = nullptr;
			}
		}

		deviceContext->PSSetShaderResources(0, job.material.TextureCount, srvs);

		// Bind the viewport to use
		D3D11_VIEWPORT vp;
		vp.Height = job.height;
		vp.Width = job.width;
		vp.TopLeftX = job.posX;
		vp.TopLeftY = job.posY;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		deviceContext->RSSetViewports(1, &vp);

		deviceContext->Draw(3, 0);

		SAFE_DELETE_ARRAY(srvs);
	}


	deviceContext->RSSetViewports(1, &fullViewport);
	return void();
}

const void Graphics::_RenderTexts()
{
	auto deviceContext = _D3D11->GetDeviceContext();
	D3D11_VIEWPORT fullViewport;
	uint32_t numViewports = 1;
	deviceContext->RSGetViewports(&numViewports, &fullViewport);

	auto backbuffer = _D3D11->GetBackBufferRTV();
	deviceContext->OMSetRenderTargets(1, &backbuffer, nullptr);

	// Set input layout
	deviceContext->IASetInputLayout(_textInputLayot);

	// Set constant buffer
	TextVSConstants vsConstants;
	DirectX::XMStoreFloat4x4(& vsConstants.Ortho, DirectX::XMMatrixTranspose( DirectX::XMLoadFloat4x4(&_orthoMatrix)));

	D3D11_MAPPED_SUBRESOURCE mappedData;
	deviceContext->Map(_textVSConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &vsConstants, sizeof(TextVSConstants));
	deviceContext->Unmap(_textVSConstantBuffer, 0);

	deviceContext->VSSetConstantBuffers(0, 1, &_textVSConstantBuffer);


	// Bind shaders
	deviceContext->VSSetShader(_textVSShader, nullptr, 0);
	deviceContext->PSSetShader(_textPSShader, nullptr, 0);
	ID3D11PixelShader *ps = _fullscreenTexturePSMultiChannel;
	//deviceContext->VSSetShader(_fullscreenTextureVS, nullptr, 0);
	//deviceContext->PSSetShader(ps, nullptr, 0);
	for (auto& j : _textJobs)
	{
		// Bind texture;

		ID3D11ShaderResourceView *SRV = _textures[j.first];
		deviceContext->PSSetShaderResources(0, 1, &SRV);


		for (auto& j2 : j.second)
		{
			// Bind buffer
			uint32_t stride = sizeof(TextVertexLayout);
			uint32_t offset = 0;
			deviceContext->IASetVertexBuffers(0, 1, &_VertexBuffers[j2.first], &stride, &offset);

			// Set constant buffer
			TextPSConstants psConstants;
			psConstants.Color = j2.second->Color;

			D3D11_MAPPED_SUBRESOURCE mappedData;
			deviceContext->Map(_textPSConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
			memcpy(mappedData.pData, &psConstants, sizeof(TextPSConstants));
			deviceContext->Unmap(_textPSConstantBuffer, 0);

			deviceContext->PSSetConstantBuffers(0, 1, &_textPSConstantBuffer);


			// Render
			deviceContext->Draw(j2.second->text.size()*6, 0);
		}
	}

}

const void Graphics::_RenderGBuffers(uint numImages) const
{

	auto deviceContext = _D3D11->GetDeviceContext();
	D3D11_VIEWPORT fullViewport;
	uint32_t numViewports = 1;
	deviceContext->RSGetViewports(&numViewports, &fullViewport);
	//
	// Full-screen textured quad
	//

	// Specify 1 or 4.
	//uint32_t numImages = 0;

	//if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000)
	//	numImages = 1;
	//else if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000)
	//	numImages = 4;

	if (numImages)
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
		for (int i = 0; i < 4; ++i)
		{
			vp[i].MinDepth = 0.0f;
			vp[i].MaxDepth = 1.0f;
			vp[i].Width = window->GetWindowWidth() / 2.0f;
			vp[i].Height = window->GetWindowHeight() / 2.0f;
			vp[i].TopLeftX = (i % 2) * window->GetWindowWidth() / 2.0f;
			vp[i].TopLeftY = (uint32_t)(0.5f * i) * window->GetWindowHeight() / 2.0f;
		}

		if (numImages == 1)
		{
			vp[0].Width = static_cast<float>(window->GetWindowWidth());
			vp[0].Height = static_cast<float>(window->GetWindowHeight());
		}

		// Here begins actual render code

		for (uint32_t i = 0; i < numImages; ++i)
		{
			ID3D11PixelShader *ps = _fullscreenTexturePSMultiChannel;
			if (srvs[i] == _mainDepth.SRV)
				ps = _fullscreenTexturePSSingleChannel;

			deviceContext->RSSetViewports(1, &vp[i]);
			deviceContext->VSSetShader(_fullscreenTextureVS, nullptr, 0);
			deviceContext->PSSetShader(ps, nullptr, 0);
			deviceContext->PSSetShaderResources(0, 1, &srvs[i]);

			deviceContext->Draw(3, 0);

			ID3D11ShaderResourceView *nullSRV = nullptr;
			deviceContext->PSSetShaderResources(0, 1, &nullSRV);
		}

		deviceContext->RSSetViewports(1, &fullViewport);
	}
	return void();
}

std::int32_t Graphics::CreateTexture( const wchar_t *filename )
{
	ID3D11ShaderResourceView *srv = nullptr;
	wstring s( filename );
	if ( s.find( L".dds", s.length() - 4 ) != wstring::npos )
	{
		if ( FAILED( DirectX::CreateDDSTextureFromFile( _D3D11->GetDevice(), filename, nullptr, &srv ) ) )
		{
			TraceDebug( "Failed to load texture: '%ls'", filename );
			throw ErrorMsg( 5000031, L"Failed to load texture.", filename );
		}
	}
	else if ( s.find( L".png", s.length() - 4 ) != wstring::npos )
	{
		if ( FAILED( DirectX::CreateWICTextureFromFile( _D3D11->GetDevice(), filename, nullptr, &srv ) ) )
		{
			TraceDebug( "Failed to load texture: '%ls'", filename );
			throw ErrorMsg( 5000031, L"Failed to load texture.", filename );
		}
	}
	else
	{
		TraceDebug( "Can't load texture: '%ls'", filename );
		throw;
	}

	_textures.push_back( srv );

	return _textures.size() - 1;
}

ID3D11Device * Graphics::GetDevice() const
{
	return _D3D11->GetDevice();
}

ID3D11DeviceContext * Graphics::GetDeviceContext() const
{
	return _D3D11->GetDeviceContext();
}

bool Graphics::_BuildInputLayout( void )
{
	//D3D11_APPEND_ALIGNED_ELEMENT kan anv�ndas p� AlignedByteOffset f�r att l�gga elementen direkt efter f�reg�ende

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

bool Graphics::_BuildTextInputLayout(void)
{

	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout.
	if (FAILED(_D3D11->GetDevice()->CreateInputLayout(vertexDesc, 2, _textShaderInput->GetBufferPointer(), _textShaderInput->GetBufferSize(), &_textInputLayot)))
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

void Graphics::AddOverlayProvider(IOverlayProvider * provider)
{
	_overlayProviders.push_back(provider);
}

void Graphics::AddLightProvider(ILightProvider* provider)
{
	_lightProviders.push_back(provider);
}

void Graphics::AddTextProvider(ITextProvider * provider)
{
	_textProviders.push_back(provider);
}

const void Graphics::ClearRenderProviders()
{
	_RenderProviders.clear();
	return void();
}

const void Graphics::ClearOverlayProviders()
{
	_overlayProviders.clear();
	return void();
}

const void Graphics::ClearCameraProviders()
{
	_cameraProviders.clear();
	return void();
}

const void Graphics::ClearLightProviders()
{
	_lightProviders.clear();
	return void();
}

const void Graphics::ClearTextProviders()
{
	_textProviders.clear();
	return void();
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
		throw ErrorMsg(5000032, L"Failed to initialize Direct3D11");

	if ( FAILED( OnCreateDevice() ) )
		throw ErrorMsg(5000033, L"Failed to create device");
	if ( FAILED( OnResizedSwapChain() ) )
		throw ErrorMsg(5000034, L"Failed to resize swap chain");

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
