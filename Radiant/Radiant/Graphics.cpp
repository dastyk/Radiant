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
	deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	_GBuffer->Clear(deviceContext);

	BeginFrame();

	// Clear depth stencil view
	deviceContext->ClearDepthStencilView(_mainDepth.DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	// Gather all the data use for rendering

	ctimer.TimeStart("GatherTot");
	_GatherRenderData(); //TODO: Add support for multithreading the gathering of data.(so that each provider can give jobs at the same time)
	ctimer.TimeEnd("GatherTot");
	// Render all the meshes provided

	_SetOncePerFrameBuffer();


	timer.TimeStart("Render");
	_RenderMeshes();
	timer.TimeEnd("Render");
	// Render lights using tiled deferred shading

	timer.TimeStart( "Effects" );
	_RenderEffects();
	timer.TimeEnd( "Effects" );

	timer.TimeStart("Decals");
	_RenderDecals();
	timer.TimeEnd("Decals");

	timer.TimeStart( "Glow" );
	_GenerateGlow();
	timer.TimeEnd( "Glow" );

	timer.TimeStart("Lights");
	_RenderLights();
	timer.TimeEnd("Lights");

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
		deviceContext->PSSetSamplers( 0, 1, &_anisoSam );

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

	_effectVS = CompileVSFromFile( device, L"Shaders/LightningVS.hlsl", "VS", "vs_4_0", nullptr, nullptr, &_effectVSByteCode );
	if ( !_effectVS )
		return E_FAIL;

	if ( !_BuildInputLayout() )
		return E_FAIL;

	_tiledDeferredCS = CompileCSFromFile( device, L"Shaders/TiledDeferred.hlsl", "CS", "cs_5_0" );
	if ( !_tiledDeferredCS )
		return E_FAIL;

	D3D_SHADER_MACRO defines[2] = { "HORIZONTAL_BLUR", "1", nullptr, nullptr };
	_separableBlurHorizontal = CompilePSFromFile( device, L"Shaders/SeparableBlur.hlsl", "PS", "ps_4_0", defines );
	_separableBlurVertical = CompilePSFromFile( device, L"Shaders/SeparableBlur.hlsl", "PS", "ps_4_0" );
	if ( !_separableBlurHorizontal || !_separableBlurVertical )
		return E_FAIL;

	_D3D11->DeleteRenderTarget( _glowTempRT1 );
	_glowTempRT1 = _D3D11->CreateRenderTarget( DXGI_FORMAT_R8G8B8A8_UNORM, 256, 256, 0, TEXTURE_COMPUTE_WRITE );
	_D3D11->DeleteRenderTarget( _glowTempRT2 );
	_glowTempRT2 = _D3D11->CreateRenderTarget( DXGI_FORMAT_R8G8B8A8_UNORM, 256, 256, 0, TEXTURE_COMPUTE_WRITE );

	_fullscreenTextureVS = CompileVSFromFile( device, L"Shaders/FullscreenTexture.hlsl", "VS", "vs_4_0" );
	_fullscreenTexturePSMultiChannel = CompilePSFromFile( device, L"Shaders/FullscreenTexture.hlsl", "PSMultiChannel", "ps_4_0" );
	_fullscreenTexturePSSingleChannel = CompilePSFromFile( device, L"Shaders/FullscreenTexture.hlsl", "PSSingleChannel", "ps_4_0" );
	_fullscreenTexturePSMultiChannelGamma = CompilePSFromFile( device, L"Shaders/FullscreenTexture.hlsl", "PSMultiChannelGamma", "ps_4_0" );
	_textVSShader = CompileVSFromFile(device, L"Shaders/TextVSShader.hlsl", "main", "vs_5_0", nullptr, nullptr, &_textShaderInput);
	_textPSShader = CompilePSFromFile(device, L"Shaders/TextPSShader.hlsl", "main", "ps_5_0");

	
	_decalsVSShader = CompileVSFromFile(device, L"Shaders/DecalsVS.hlsl", "VS", "vs_5_0", nullptr, nullptr, &_decalShaderInput);
	_decalsPSShader = CompilePSFromFile(device, L"Shaders/DecalsPS.hlsl", "PS", "ps_5_0");

	_DecalData = _CreateDecalData();
	if (!_BuildDecalInputLayout())
		return E_FAIL;

	if ( !_fullscreenTextureVS || !_fullscreenTexturePSMultiChannel || !_fullscreenTexturePSSingleChannel || !_textVSShader || !_textPSShader)
		return E_FAIL;

	if (!_BuildTextInputLayout())
		return E_FAIL;

	D3D11_BUFFER_DESC bufDesc;
	memset( &bufDesc, 0, sizeof( D3D11_BUFFER_DESC ) );
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	

	bufDesc.ByteWidth = sizeof(Graphics::OncePerFrameConstants);
	HR_RETURN(device->CreateBuffer(&bufDesc, nullptr, &_oncePerFrameConstantsBuffer));


	bufDesc.ByteWidth = sizeof( Graphics::StaticMeshVSConstants );
	HR_RETURN( device->CreateBuffer( &bufDesc, nullptr, &_staticMeshVSConstants ) );

	bufDesc.ByteWidth = sizeof( Graphics::TiledDeferredConstants );
	HR_RETURN( device->CreateBuffer( &bufDesc, nullptr, &_tiledDeferredConstants ) );

	//bufDesc.ByteWidth = sizeof(Graphics::TextVSConstants);
	//HR_RETURN(device->CreateBuffer(&bufDesc, nullptr, &_textVSConstantBuffer));

	bufDesc.ByteWidth = sizeof(Graphics::TextPSConstants);
	HR_RETURN(device->CreateBuffer(&bufDesc, nullptr, &_textPSConstantBuffer));

	bufDesc.ByteWidth = sizeof( float ) * 4; // Two floats plus two pad
	HR_RETURN( device->CreateBuffer( &bufDesc, nullptr, &_blurTexelConstants ) );
	bufDesc.ByteWidth = sizeof(Graphics::DecalsVSConstantBuffer);
	HR_RETURN(device->CreateBuffer(&bufDesc, nullptr, &_decalsVSConstants));

	//bufDesc.ByteWidth = sizeof(Graphics::DecalsConstantBuffer);
	//HR_RETURN(device->CreateBuffer(&bufDesc, nullptr, &_decalsPSConstants));

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
	HR_RETURN( device->CreateSamplerState( &samDesc, &_anisoSam ) );

	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samDesc.BorderColor[0] = 0.0f;
	samDesc.BorderColor[1] = 0.0f;
	samDesc.BorderColor[2] = 0.0f;
	samDesc.BorderColor[3] = 0.0f;
	samDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.MinLOD = -FLT_MAX;
	samDesc.MaxLOD = FLT_MAX;
	samDesc.MipLODBias = 0.0f;
	HR_RETURN( device->CreateSamplerState( &samDesc, &_triLinearSam ) );

	_pointLightsBuffer = _D3D11->CreateStructuredBuffer( sizeof( PointLight ), 1024 );
	_spotLightsBuffer = _D3D11->CreateStructuredBuffer( sizeof( SpotLight ), 1024 );
	_capsuleLightsBuffer = _D3D11->CreateStructuredBuffer( sizeof( CapsuleLight ), 1024 );
	_areaRectLightBuffer = _D3D11->CreateStructuredBuffer(sizeof(AreaRectLight), 1024);

	

	_PointLightData = _CreatePointLightData(0);
	_SpotLightData = _CreateSpotLightData(3);

	_lightVertexShader = CompileVSFromFile(device, L"Shaders/LightVS.hlsl", "main", "vs_5_0", nullptr, nullptr, &_lightShaderInput);
	_lightBackFacePixelShader = CompilePSFromFile(device, L"Shaders/LightBackPS.hlsl", "main", "ps_5_0");
	_lightFrontFacePixelShader = CompilePSFromFile(device, L"Shaders/LightFrontPS.hlsl", "main", "ps_5_0");
	_lightFinalPixelShader = CompilePSFromFile(device, L"Shaders/LightFinalPS.hlsl", "main", "ps_5_0");

	if (!_BuildLightInputLayout())
		return E_FAIL;

	_dssWriteToDepthDisabled = _D3D11->CreateDepthStencilState(true, D3D11_DEPTH_WRITE_MASK_ZERO);
	_dssWriteToDepthEnabled = _D3D11->CreateDepthStencilState(true, D3D11_DEPTH_WRITE_MASK_ALL);

	_rsBackFaceCullingEnabled = _D3D11->CreateRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_FRONT);
	_rsFrontFaceCullingEnabled = _D3D11->CreateRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_BACK);
	_rsFaceCullingDisabled = _D3D11->CreateRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_NONE);


	_bsBlendEnabled = _D3D11->CreateBlendState(true);
	_bsBlendDisabled = _D3D11->CreateBlendState(false);

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

	SAFE_RELEASE( _effectVS );
	SAFE_RELEASE( _effectInputLayout );
	SAFE_RELEASE( _effectVSByteCode );

	SAFE_RELEASE( _fullscreenTextureVS );
	SAFE_RELEASE( _fullscreenTexturePSMultiChannel );
	SAFE_RELEASE( _fullscreenTexturePSSingleChannel );
	SAFE_RELEASE( _fullscreenTexturePSMultiChannelGamma );

	SAFE_RELEASE(_textVSShader);
	SAFE_RELEASE(_textPSShader);
	SAFE_RELEASE(_textShaderInput);
	SAFE_RELEASE(_textInputLayout);
//	SAFE_RELEASE(_textVSConstantBuffer);
	SAFE_RELEASE(_textPSConstantBuffer);
	
	SAFE_RELEASE(_oncePerFrameConstantsBuffer);
	SAFE_RELEASE( _separableBlurHorizontal );
	SAFE_RELEASE( _separableBlurVertical );
	SAFE_RELEASE( _blurTexelConstants );
	_D3D11->DeleteRenderTarget( _glowTempRT1 );
	_D3D11->DeleteRenderTarget( _glowTempRT2 );
	SAFE_RELEASE(_decalsVSConstants);
	//SAFE_RELEASE(_decalsPSConstants);
	SAFE_RELEASE(_decalsVSShader);
	SAFE_RELEASE(_decalsPSShader);
	_DeleteDecalData(_DecalData);
	SAFE_RELEASE(_decalsInputLayout);
	SAFE_RELEASE(_decalShaderInput);

	_D3D11->DeleteStructuredBuffer( _pointLightsBuffer );
	_D3D11->DeleteStructuredBuffer(_spotLightsBuffer);
	_D3D11->DeleteStructuredBuffer( _spotLightsBuffer );
	_D3D11->DeleteStructuredBuffer( _capsuleLightsBuffer );
	_D3D11->DeleteStructuredBuffer(_areaRectLightBuffer);


	for ( auto srv : _textures )
	{
		SAFE_RELEASE( srv.second);
	}

	for ( ID3D11Buffer *b : _VertexBuffers )
	{
		SAFE_RELEASE( b );
	}

	for ( ID3D11Buffer *b : _IndexBuffers )
	{
		SAFE_RELEASE( b );
	}

	for (auto& b : _DynamicVertexBuffers)
		_DeleteDynamicVertexBuffer(b);

	for ( auto& b : _dynamicStructuredBuffers )
		_D3D11->DeleteStructuredBuffer( b );

	SAFE_RELEASE( _materialConstants );
	for ( auto s : _materialShaders )
		SAFE_RELEASE( s );

	SAFE_RELEASE( _anisoSam );
	SAFE_RELEASE( _triLinearSam );


	_DeletePointLightData(_PointLightData);
	_DeleteSpotLightData(_SpotLightData);

	SAFE_RELEASE(_lightVertexShader);
	SAFE_RELEASE(_lightBackFacePixelShader);
	SAFE_RELEASE(_lightFrontFacePixelShader);
	SAFE_RELEASE(_lightFinalPixelShader);
	SAFE_RELEASE(_lightInputLayout);
	SAFE_RELEASE(_lightShaderInput);

	_D3D11->DeleteDepthStencilState(_dssWriteToDepthDisabled);
	_D3D11->DeleteDepthStencilState(_dssWriteToDepthEnabled);

	_D3D11->DeleteRasterizerState(_rsBackFaceCullingEnabled);
	_D3D11->DeleteRasterizerState(_rsFrontFaceCullingEnabled);
	_D3D11->DeleteRasterizerState(_rsFaceCullingDisabled);

	_D3D11->DeleteBlendState(_bsBlendEnabled);
	_D3D11->DeleteBlendState(_bsBlendDisabled);
}


HRESULT Graphics::OnResizedSwapChain( void )
{
	auto device = _D3D11->GetDevice();
	auto context = _D3D11->GetDeviceContext();
	float width = 0;
	float height = 0;
	auto o = System::GetOptions();
	width = (float)o->GetScreenResolutionWidth();
	height = (float)o->GetScreenResolutionHeight();

	_D3D11->Resize(static_cast<unsigned int>(width), static_cast<unsigned int>(height));

	_D3D11->DeleteDepthBuffer(_mainDepth);
	_mainDepth = _D3D11->CreateDepthBuffer( DXGI_FORMAT_D24_UNORM_S8_UINT, static_cast<unsigned int>(width), static_cast<unsigned int>(height), true );
	_D3D11->DeleteRenderTarget(_accumulateRT);
	_accumulateRT = _D3D11->CreateRenderTarget( DXGI_FORMAT_R16G16B16A16_FLOAT, static_cast<unsigned int>(width), static_cast<unsigned int>(height), 0, TEXTURE_COMPUTE_WRITE );

	SAFE_RELEASE( _downSamplePS );
	string hkernel_half = to_string( (int)ceilf( width / 256.0f / 2.0f ) );
	string vkernel_half = to_string( (int)ceilf( height / 256.0f / 2.0f ) );
	string texelWidth = to_string( 1.0f / width ).c_str();
	string texelHeight = to_string( 1.0f / height ).c_str();
	D3D_SHADER_MACRO defines[] = {
		"HKERNEL_HALF", hkernel_half.c_str(),
		"VKERNEL_HALF", vkernel_half.c_str(),
		"TEXEL_WIDTH", texelWidth.c_str(),
		"TEXEL_HEIGHT", texelHeight.c_str(),
		nullptr, nullptr };
	_downSamplePS = CompilePSFromFile( device, L"Shaders/DownSample.hlsl", "PS", "ps_4_0", defines );
	if ( !_downSamplePS )
		return E_FAIL;

	SAFE_DELETE(_GBuffer);
	_GBuffer = new GBuffer( device, static_cast<uint32_t>(width), static_cast<uint32_t>(height));


	DirectX::XMStoreFloat4x4(&_orthoMatrix, DirectX::XMMatrixOrthographicLH(width, height, 0.001f, 10.0f));

	return S_OK;
}


void Graphics::OnReleasingSwapChain( void )
{
	_D3D11->DeleteDepthBuffer( _mainDepth );

	_D3D11->DeleteRenderTarget( _accumulateRT );

	SAFE_RELEASE( _downSamplePS );

	SAFE_DELETE( _GBuffer );
}


uint Graphics::CreateTextBuffer(FontData* data)
{
	void *vertexData = nullptr;
	uint32_t vertexDataSize = 0;
	_BuildVertexData(data, (TextVertexLayout**)&vertexData, vertexDataSize);
	//vertexDataSize = 1024 * 6 * sizeof(TextVertexLayout);
	DynamicVertexBuffer vertexBuffer;
	try { vertexBuffer = _CreateDynamicVertexBuffer(vertexData, vertexDataSize); }
	catch (ErrorMsg& msg)
	{
		SAFE_DELETE_ARRAY(vertexData);
		throw msg;
	}
	SAFE_DELETE_ARRAY(vertexData);

	_DynamicVertexBuffers.push_back(vertexBuffer);
	return static_cast<unsigned int>(_DynamicVertexBuffers.size() - 1);
}

const void Graphics::UpdateTextBuffer(FontData* data)
{

	void *vertexData = nullptr;
	uint32_t vertexDataSize = 0;
	_BuildVertexData(data,(TextVertexLayout**)&vertexData, vertexDataSize);
	_MapDataToDynamicVertexBuffer(_DynamicVertexBuffers[data->VertexBuffer], vertexData, vertexDataSize);
	SAFE_DELETE_ARRAY(vertexData);
	return void();
}

uint32_t Graphics::CreateDynamicVertexBuffer( void )
{
	DynamicVertexBuffer buf;
	buf.buffer = nullptr;
	buf.size = 0;

	_DynamicVertexBuffers.push_back( buf );

	return static_cast<uint32_t>(_DynamicVertexBuffers.size() - 1);
}

void Graphics::UpdateDynamicVertexBuffer( uint32_t bufferIndex, void *data, uint32_t bytes )
{
	DynamicVertexBuffer& buf = _DynamicVertexBuffers[bufferIndex];

	_MapDataToDynamicVertexBuffer( buf, data, bytes );
}

TextureProxy Graphics::CreateDynamicStructuredBuffer( uint32_t stride )
{
	StructuredBuffer buf = _D3D11->CreateStructuredBuffer( stride, 0, true );
	_dynamicStructuredBuffers.push_back( buf );

	return TextureProxy( TextureProxy::Type::StructuredDynamic, static_cast<signed>(_dynamicStructuredBuffers.size()) - 1 );
}

void Graphics::UpdateDynamicStructuredBuffer( TextureProxy buffer, void *data, uint32_t stride, uint32_t elementCount )
{
	StructuredBuffer& buf = _dynamicStructuredBuffers[buffer.Index];

	_MapDataToDynamicStructuredBuffer( buf, data, stride, elementCount );
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

const Graphics::DynamicVertexBuffer Graphics::_CreateDynamicVertexBuffer(void * vertexData, std::uint32_t vertexDataSize)const
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
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	DynamicVertexBuffer buf;
	buf.size = vertexDataSize;
	if (FAILED(_D3D11->GetDevice()->CreateBuffer(&bufDesc, vertexData ? &initData : nullptr, &buf.buffer)))
		throw ErrorMsg(5000037, L"Failed to create Dynamic Vertex Buffer.");

	return buf;
}

const void Graphics::_DeleteDynamicVertexBuffer(DynamicVertexBuffer & buffer) const
{
	SAFE_RELEASE(buffer.buffer);
	return void();
}

const bool Graphics::_ResizeDynamicVertexBuffer(DynamicVertexBuffer & buffer, void * vertexData, std::uint32_t& vertexDataSize) const
{
	DynamicVertexBuffer buf;

	try { buf = _CreateDynamicVertexBuffer(vertexData, vertexDataSize); }
	catch (ErrorMsg& msg)
	{
		msg;
		_DeleteDynamicVertexBuffer(buf);
		vertexDataSize = buffer.size;
		TraceDebug("Failed to resize dynamic vertex buffer, size was cut.");
		return false;
	}
	_DeleteDynamicVertexBuffer(buffer);
	buffer.buffer = buf.buffer;
	buffer.size = buf.size;
	return true;
}

bool Graphics::_ResizeDynamicStructuredBuffer( StructuredBuffer& buffer, void *data, std::uint32_t stride, std::uint32_t numElements ) const
{
	_D3D11->DeleteStructuredBuffer( buffer );
	buffer = _D3D11->CreateStructuredBuffer( stride, numElements, true, false, data );
	return true;
}

const void Graphics::_MapDataToDynamicVertexBuffer(DynamicVertexBuffer & buffer, void * vertexData, std::uint32_t vertexDataSize) const
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	if (vertexDataSize > buffer.size)
		if (_ResizeDynamicVertexBuffer(buffer, vertexData, vertexDataSize))
			return;

	// Lock the vertex buffer so it can be written to.
	if (FAILED(_D3D11->GetDeviceContext()->Map(buffer.buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		TraceDebug("Failed to map text to vertex buffer.");
		return;
	}

	// Copy the data into the vertex buffer.
	memcpy(mappedResource.pData, (void*)vertexData, vertexDataSize);

	// Unlock the vertex buffer.
	_D3D11->GetDeviceContext()->Unmap(buffer.buffer, 0);
	return void();
}

void Graphics::_MapDataToDynamicStructuredBuffer( StructuredBuffer& buffer, void* data, uint32_t stride, uint32_t numElements ) const
{
	if ( stride * numElements > buffer.Stride * buffer.ElementCount || stride != buffer.Stride || numElements != buffer.ElementCount )
	{
		_ResizeDynamicStructuredBuffer( buffer, data, stride, numElements );
		return;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	_D3D11->GetDeviceContext()->Map( buffer.Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	memcpy( mappedResource.pData, data, stride * numElements );
	_D3D11->GetDeviceContext()->Unmap( buffer.Buffer, 0 );
}

const void Graphics::_BuildVertexData(FontData* data, TextVertexLayout** vertexPtr, uint32_t& vertexDataSize)
{
	uint numLetters, index, i, letter;
	float drawX, drawY;
	drawX = DirectX::XMVectorGetX(DirectX::XMLoadFloat3(&data->pos));
	drawY = -DirectX::XMVectorGetY(DirectX::XMLoadFloat3(&data->pos));

	// Get the number of letters in the sentence.
	numLetters = (uint)data->text.size();
	vertexDataSize = sizeof(TextVertexLayout)* numLetters * 6;
	(*vertexPtr) = new TextVertexLayout[numLetters * 6];
	memset((*vertexPtr), 0, sizeof(TextVertexLayout)* numLetters * 6);

	// Initialize the index to the vertex array.
	index = 0;

	// Draw each letter onto a quad.
	for (i = 0; i < numLetters; i++)
	{
		letter = ((uint)data->text[i]);
		if (letter == 10)
		{
			drawX = DirectX::XMVectorGetX(DirectX::XMLoadFloat3(&data->pos));
			drawY -= (float)data->font->refSize*data->FontSize;
			vertexDataSize -= sizeof(TextVertexLayout) * 6;
		}
		// If the letter is a space then just move over.
		else if (letter == 32)
		{
			drawX = drawX + (uint)((float)data->font->refSize*data->FontSize*0.4);
			vertexDataSize -= sizeof(TextVertexLayout) * 6;
		}
		else
		{
			
			// First triangle in quad.
			(*vertexPtr)[index]._position = XMFLOAT3(drawX, drawY - ((float)data->FontSize*(float)data->font->refSize), 1.0f);  // Bottom left
			(*vertexPtr)[index]._texCoords = XMFLOAT2(data->font->Font[letter].left, 1.0f);
			index++;

			(*vertexPtr)[index]._position = XMFLOAT3(drawX, drawY, 1.0f);  // Top left
			(*vertexPtr)[index]._texCoords = XMFLOAT2(data->font->Font[letter].left, 0.0f);
			index++;

			(*vertexPtr)[index]._position = XMFLOAT3(drawX + ((float)data->font->Font[letter].size*(float)data->FontSize), drawY, 1.0f);  // Top right.
			(*vertexPtr)[index]._texCoords = XMFLOAT2(data->font->Font[letter].right, 0.0f);
			index++;

			// Second triangle in quad.
			(*vertexPtr)[index]._position = XMFLOAT3((drawX + ((float)data->font->Font[letter].size)*(float)data->FontSize), (drawY - ((float)data->FontSize)*(float)data->font->refSize), 1.0f);  // Bottom right.
			(*vertexPtr)[index]._texCoords = XMFLOAT2(data->font->Font[letter].right, 1.0f);
			index++;

			(*vertexPtr)[index]._position = XMFLOAT3(drawX, drawY - ((float)data->FontSize*(float)data->font->refSize), 1.0f);  // bottom left.
			(*vertexPtr)[index]._texCoords = XMFLOAT2(data->font->Font[letter].left, 1.0f);
			index++;

			(*vertexPtr)[index]._position = XMFLOAT3(drawX + ((float)data->font->Font[letter].size*(float)data->FontSize), drawY, 1.0f);  // Top right.
			(*vertexPtr)[index]._texCoords = XMFLOAT2(data->font->Font[letter].right, 0.0f);
			index++;

	

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + (uint)((data->font->Font[letter].size)*(float)data->FontSize + 1.0f);
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

bool Graphics::_CreateMeshBuffers(Mesh * mesh, std::uint32_t & vertexBufferIndex, std::uint32_t & indexBufferIndex)
{
	void *vertexData = nullptr;
	uint32_t vertexDataSize = 0;
	void *indexData = nullptr;
	uint32_t indexDataSize = 0;
	_InterleaveVertexData(mesh, &vertexData, vertexDataSize, &indexData, indexDataSize);

	ID3D11Buffer *vertexBuffer = _CreateVertexBuffer(vertexData, vertexDataSize);
	ID3D11Buffer *indexBuffer = _CreateIndexBuffer(indexData, indexDataSize);
	if (!vertexBuffer || !indexBuffer)
	{
		SAFE_DELETE_ARRAY(vertexData);
		SAFE_DELETE_ARRAY(indexData);
		SAFE_RELEASE(vertexBuffer);
		SAFE_RELEASE(indexBuffer);
		return false;
	}
	SAFE_DELETE_ARRAY(vertexData);
	SAFE_DELETE_ARRAY(indexData);

	_VertexBuffers.push_back(vertexBuffer);
	vertexBufferIndex = static_cast<unsigned int>(_VertexBuffers.size() - 1);
	_IndexBuffers.push_back(indexBuffer);
	indexBufferIndex = static_cast<unsigned int>(_IndexBuffers.size() - 1);

	return true;
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
	auto materialShader = CompilePSFromFile( _D3D11->GetDevice(), shaderFile, "PS", "ps_5_0", nullptr, nullptr, &byteCode );

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
		if ( bindDesc.Type == D3D_SIT_TEXTURE || bindDesc.Type == D3D_SIT_STRUCTURED )
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
		material.Textures = new TextureProxy[maxRegister + 1];

		for ( int32_t i = 0; i <= maxRegister; ++i )
		{
			material.Textures[i].Type = TextureProxy::Type::Regular;
			material.Textures[i].Index = 1;
		}
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

const void Graphics::_SetOncePerFrameBuffer()
{
	auto deviceContext = _D3D11->GetDeviceContext();
	auto o = System::GetOptions();
	OncePerFrameConstants data;

	XMMATRIX view, proj, viewproj, invView, invProj, invViewProj, ortho;

	view = XMLoadFloat4x4(&_renderCamera->viewMatrix);
	proj = XMLoadFloat4x4(&_renderCamera->projectionMatrix);
	viewproj = XMLoadFloat4x4(&_renderCamera->viewProjectionMatrix);

	invView = XMMatrixInverse(nullptr, view);
	invProj = XMMatrixInverse(nullptr, proj);
	invViewProj = XMMatrixInverse(nullptr, viewproj);

	ortho = XMLoadFloat4x4(&_orthoMatrix);

	XMStoreFloat4x4(&data.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&data.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&data.ViewProj, XMMatrixTranspose(viewproj));

	XMStoreFloat4x4(&data.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&data.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&data.InvViewProj, XMMatrixTranspose(invViewProj));

	XMStoreFloat4x4(&data.Ortho, XMMatrixTranspose(ortho));

	data.CameraPosition = _renderCamera->camPos;
	data.DrawDistance = _renderCamera->farp;
	data.ViewDistance = _renderCamera->viewDistance;

	data.BackbufferWidth = static_cast<float>(o->GetScreenResolutionWidth());
	data.BackbufferHeight = static_cast<float>(o->GetScreenResolutionHeight());

	D3D11_MAPPED_SUBRESOURCE mappedsubres;
	deviceContext->Map(_oncePerFrameConstantsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedsubres);
	memcpy(mappedsubres.pData, &data, sizeof(OncePerFrameConstants));
	deviceContext->Unmap(_oncePerFrameConstantsBuffer, 0);

	deviceContext->PSSetConstantBuffers(0, 1, &_oncePerFrameConstantsBuffer);
	deviceContext->VSSetConstantBuffers(0, 1, &_oncePerFrameConstantsBuffer);
	deviceContext->CSSetConstantBuffers(0, 1, &_oncePerFrameConstantsBuffer);

}

const void Graphics::_GatherRenderData()
{
	// Gather meshes to render
	ctimer.TimeStart("Mesh");
	_renderJobs.clear();
	for (auto renderProvider : _RenderProviders)
		renderProvider->GatherJobs(_renderJobs);
	ctimer.TimeEnd("Mesh");

	// Lights
	ctimer.TimeStart("Lights");
	_pointLights.clear();
	_spotLights.clear();
	_capsuleLights.clear();
	_areaRectLights.clear();
	for ( auto lightProvider : _lightProviders )
		lightProvider->GatherLights(_pointLights, _spotLights, _capsuleLights, _areaRectLights);
	ctimer.TimeEnd("Lights");

	ctimer.TimeStart("Camera");
	// Get the current camera
	for (auto camProvider : _cameraProviders)
		camProvider->GatherCam(_renderCamera);
	ctimer.TimeEnd("Camera");

	ctimer.TimeStart("Overlay");
	// Gather overlaydata
	_overlayRenderJobs.clear();
	for (auto overlayprovider : _overlayProviders)
	{
		overlayprovider->GatherOverlayJobs([this](OverlayData* data) -> void
		{
			_overlayRenderJobs.push_back(data);
		});
	}
	ctimer.TimeEnd("Overlay");

	ctimer.TimeStart("Texts");
	_textJobs.clear();

	for (auto textprovider : _textProviders)
		textprovider->GatherTextJobs(_textJobs);
	ctimer.TimeEnd("Texts");

	ctimer.TimeStart( "Effects" );
	_effects.clear();
	for ( auto effectProvider : _effectProviders )
		effectProvider->GatherEffects( _effects );
	ctimer.TimeEnd( "Effects" );

	_decals.clear();
	_decalGroups.clear();
	ctimer.TimeStart("Decals");
	for (auto decalprovider : _decalProviders)
		decalprovider->GatherDecals(_decals, _decalGroups);
	ctimer.TimeEnd("Decals");

	return void();
}


const void Graphics::_RenderDecals()
{

	if (_decals.size() > 0)
	{
		ID3D11ShaderResourceView *srvs2[128];
		auto deviceContext = _D3D11->GetDeviceContext();
		auto device = _D3D11->GetDevice();
		//We dont cull backfaces for this since we might be standing inside the decals box
		deviceContext->RSSetState(_rsFaceCullingDisabled.RS);
	
		//bla bla comment
		ID3D11RenderTargetView* rtvs[] = { _GBuffer->ColorRT(), _GBuffer->NormalRT(), _GBuffer->EmissiveRT() };
		deviceContext->OMSetRenderTargets(3, rtvs, nullptr);
	
	
		ID3D11ShaderResourceView* srvs[] = { _mainDepth.SRV }; //Use depth to get position
		deviceContext->PSSetShaderResources(0, 1, srvs);

		deviceContext->IASetInputLayout(_decalsInputLayout);
	
		//DecalsConstantBuffer dcb;
		XMMATRIX ViewProj = XMLoadFloat4x4(&_renderCamera->viewProjectionMatrix);
		XMMATRIX invViewProj = XMMatrixInverse(nullptr, ViewProj);
		//XMStoreFloat4x4(&dcb.invViewProj, XMMatrixTranspose(invViewProj));	
		XMMATRIX View = XMLoadFloat4x4(&_renderCamera->viewMatrix);
		//XMStoreFloat4x4(&dcb.View, XMMatrixTranspose(View));
		/*
		D3D11_MAPPED_SUBRESOURCE mappedsubres;
		deviceContext->Map(_decalsPSConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedsubres);
		memcpy(mappedsubres.pData, &dcb, sizeof(DecalsConstantBuffer));
		deviceContext->Unmap(_decalsPSConstants, 0);
		deviceContext->PSSetConstantBuffers(1, 1, &_decalsPSConstants);*/
	

		deviceContext->VSSetShader(_decalsVSShader, nullptr, 0);
		deviceContext->PSSetShader(_decalsPSShader, nullptr, 0);
		deviceContext->PSSetSamplers(0, 1, &_triLinearSam);

		uint32_t stride = sizeof(DecalLayout);
		uint32_t offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, &_DecalData.vertexbuffer, &stride, &offset);
		
		deviceContext->IASetIndexBuffer(_DecalData.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//deviceContext->OMSetBlendState(_bsBlendEnabled.BS, nullptr, ~0U);
			
		for (auto &decalgroups : _decalGroups)
		{
	
			DecalsPerObjectBuffer dpob;
			DecalsVSConstantBuffer dvscb;
			D3D11_MAPPED_SUBRESOURCE md;
	
			deviceContext->Map(_materialConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &md);
			memcpy(md.pData, _decals[decalgroups->indexStart]->shaderData->ConstantsMemory, _decals[decalgroups->indexStart]->shaderData->ConstantsMemorySize);
			deviceContext->Unmap(_materialConstants, 0);
			deviceContext->PSSetConstantBuffers(1, 1, &_materialConstants);
		
			for (int i = 0; i < static_cast<int>(decalgroups->indexCount); ++i)
			{
				//The invWorld of the decal box
				XMMATRIX World = XMLoadFloat4x4(_decals[i + decalgroups->indexStart]->World);
				XMMATRIX invWorld = XMMatrixInverse(nullptr, World);
				invWorld = XMMatrixTranspose(invWorld);
				XMStoreFloat4x4(&dpob.invWorld[i], invWorld);

				//WorldViewProj for vertex shader
				XMMATRIX WorldViewProj = XMMatrixTranspose(World * ViewProj);
				XMStoreFloat4x4(&dvscb.WorldViewProj[i], WorldViewProj);
			}

			deviceContext->Map(_decalsVSConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &md);
			memcpy(md.pData, &dvscb, sizeof(DecalsVSConstantBuffer));
			deviceContext->Unmap(_decalsVSConstants, 0);
			deviceContext->VSSetConstantBuffers(1, 1, &_decalsVSConstants);

			deviceContext->Map(_DecalData.constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &md);
			memcpy(md.pData, &dpob, sizeof(DecalsPerObjectBuffer));
			deviceContext->Unmap(_DecalData.constantBuffer, 0);
			deviceContext->PSSetConstantBuffers(2, 1, &_DecalData.constantBuffer);
			
			// Find the actual srvs to use.
			auto& tex = _texWrappers[_decals[decalgroups->indexStart]->shaderData->TextureWrapp];

			for (uint32_t i = 0; i < tex.size(); ++i)
			{
				TextureProxy& texture = tex[i];
				if (texture.Index == 1)
				{
					srvs2[i] = nullptr;
				}
				else
				{
					switch (texture.Type)
					{
					case TextureProxy::Type::Regular:
						srvs2[i] = _textures[texture.Index];
						break;

					case TextureProxy::Type::Structured:
						srvs2[i] = nullptr;
						break;

					case TextureProxy::Type::StructuredDynamic:
						srvs2[i] = _dynamicStructuredBuffers[texture.Index].SRV;
						break;

					default:
						srvs2[i] = nullptr;
					}
				}
			}


			deviceContext->PSSetShaderResources(1, (UINT)tex.size(), srvs2);
			deviceContext->DrawIndexedInstanced(_DecalData.indexCount, decalgroups->indexCount, 0, 0, decalgroups->indexStart);
		}
		ID3D11ShaderResourceView* nullsrvs[] = { nullptr, nullptr, nullptr, nullptr };
		deviceContext->PSSetShaderResources(0, 4, nullsrvs);
	}
	return void();
}

const void Graphics::_RenderMeshes()
{
	auto deviceContext = _D3D11->GetDeviceContext();
	ID3D11ShaderResourceView *srvs[128];


	// Enable depth testing when rendering scene.
	ID3D11RenderTargetView *rtvs[] = { _GBuffer->ColorRT(), _GBuffer->NormalRT(), _GBuffer->EmissiveRT(), _GBuffer->DepthRT() };

	float color[] = { 0.0f,0.0f,0.0f,0.0f };

	deviceContext->ClearRenderTargetView(rtvs[3], color);

	deviceContext->OMSetRenderTargets(4, rtvs, _mainDepth.DSV);
	StaticMeshVSConstants vsConstants;
	D3D11_MAPPED_SUBRESOURCE mappedData;
	// Render the scene
	{
		deviceContext->IASetInputLayout(_inputLayout);

		XMMATRIX world, worldView, wvp, worldViewInvTrp, view, viewproj;

		view = DirectX::XMLoadFloat4x4(&_renderCamera->viewMatrix);
		viewproj = DirectX::XMLoadFloat4x4(&_renderCamera->viewProjectionMatrix);
		deviceContext->VSSetShader(_staticMeshVS, nullptr, 0);
		deviceContext->PSSetShader(_materialShaders[_defaultMaterial.Shader], nullptr, 0);
		deviceContext->PSSetSamplers(0, 1, &_anisoSam);
		for (auto& shader : _renderJobs)
		{
			uint32_t stride = sizeof(VertexLayout);
			uint32_t offset = 0;
			deviceContext->PSSetShader(_materialShaders[shader.first], nullptr, 0);

			for (auto& buf : shader.second)
			{
				deviceContext->IASetVertexBuffers(0, 1, &_VertexBuffers[_MeshBuffers[buf.first].VertexBuffer], &stride, &offset);


				deviceContext->IASetIndexBuffer(_IndexBuffers[_MeshBuffers[buf.first].VertexBuffer], DXGI_FORMAT_R32_UINT, 0);
				for (auto& textures : buf.second)
				{

					// Find the actual srvs to use.
					auto& tex = _texWrappers[textures.first];

					for (uint32_t i = 0; i < tex.size(); ++i)
					{
						TextureProxy& texture = tex[i];
						if (texture.Index == 1)
						{
							srvs[i] = nullptr;
						}
						else
						{
							switch (texture.Type)
							{
							case TextureProxy::Type::Regular:
								srvs[i] = _textures[texture.Index];
								break;

							case TextureProxy::Type::Structured:
								srvs[i] = nullptr;
								break;

							case TextureProxy::Type::StructuredDynamic:
								srvs[i] = _dynamicStructuredBuffers[texture.Index].SRV;
								break;

							default:
								srvs[i] = nullptr;
							}
						}
					}


					deviceContext->PSSetShaderResources(0, (UINT)tex.size(), srvs);


					for (RenderJobMap4::iterator it = textures.second.begin(); it != textures.second.end(); ++it)
					{

						world = DirectX::XMLoadFloat4x4((*it)->translation);


						worldView = world * view;
						// Don't forget to transpose matrices that go to the shader. This was
						// handled behind the scenes in effects framework. The reason for this
						// is that HLSL uses column major matrices whereas DirectXMath uses row
						// major. If one forgets to transpose matrices, when HLSL attempts to
						// read a column it's really a row.
						wvp = XMMatrixTranspose(world * viewproj);
						worldViewInvTrp = XMMatrixInverse(nullptr, worldView); // Normally transposed, but since it's done again for shader I just skip it

																			   // Set object specific constants.

						DirectX::XMStoreFloat4x4(&vsConstants.WVP, wvp);
						DirectX::XMStoreFloat4x4(&vsConstants.WorldViewInvTrp, worldViewInvTrp);
						DirectX::XMStoreFloat4x4(&vsConstants.World, XMMatrixTranspose(world));
						//	DirectX::XMStoreFloat4(&vsConstants.CameraPosition, camPos);



						// Update shader constants.

						deviceContext->Map(_staticMeshVSConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
						memcpy(mappedData.pData, &vsConstants, sizeof(StaticMeshVSConstants));
						deviceContext->Unmap(_staticMeshVSConstants, 0);


						// TODO: Put the material in as the hash value for the job map, so that we only need to bind the material, and textures once per frame. Instead of once per mesh part.
						// Basiclly sorting after material aswell // if we define a max texture count in the shader, we can easily do an insertion sort.(like we have now)

						// TODO: Also make sure that we were given enough materials. If there is no material
						// for this mesh we can use a default one.
						//deviceContext->PSSetShader( _materialShaders[_defaultMaterial.Shader], nullptr, 0 );
						deviceContext->Map(_materialConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
						memcpy(mappedData.pData, (*it)->Material->ConstantsMemory, (*it)->Material->ConstantsMemorySize);
						deviceContext->Unmap(_materialConstants, 0);

						deviceContext->VSSetConstantBuffers(1, 1, &_staticMeshVSConstants);


						deviceContext->PSSetConstantBuffers(1, 1, &_materialConstants);



						deviceContext->DrawIndexed((*it)->IndexCount, (*it)->IndexStart, 0);
					}


				}

			}

		}
	}



	return void();
}

void Graphics::_RenderEffects()
{
	ID3D11DeviceContext *context = _D3D11->GetDeviceContext();

	ID3D11RenderTargetView *rtv = _GBuffer->EmissiveRT();
	context->OMSetRenderTargets( 1, &rtv, _mainDepth.DSVReadOnly );
	context->OMSetBlendState( _bsBlendEnabled.BS, nullptr, ~0U );
	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
	context->IASetInputLayout( _effectInputLayout );

	StaticMeshVSConstants vsConstants;
	DirectX::XMStoreFloat4x4( &vsConstants.WVP, XMMatrixTranspose( XMLoadFloat4x4( &_renderCamera->viewProjectionMatrix ) ) );
	DirectX::XMStoreFloat4x4( &vsConstants.WorldViewInvTrp, XMMatrixIdentity() ); // Not used in shader
	DirectX::XMStoreFloat4x4( &vsConstants.World, XMMatrixIdentity() ); // Not used in shader
	DirectX::XMStoreFloat4x4( &vsConstants.WorldView, XMMatrixIdentity() ); // Not used in shader

	// Update shader constants.
	D3D11_MAPPED_SUBRESOURCE mappedData;
	context->Map( _staticMeshVSConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData );
	memcpy( mappedData.pData, &vsConstants, sizeof( StaticMeshVSConstants ) );
	context->Unmap( _staticMeshVSConstants, 0 );

	context->IASetIndexBuffer( nullptr, DXGI_FORMAT_UNKNOWN, 0 );
	context->VSSetShader( _effectVS, nullptr, 0 );
	context->VSSetConstantBuffers( 1, 1, &_staticMeshVSConstants );

	uint32_t stride = sizeof( XMFLOAT3 );
	uint32_t offset = 0;

	for ( auto& effect : _effects )
	{
		context->Map( _materialConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData );
		memcpy( mappedData.pData, effect.Material->ConstantsMemory, effect.Material->ConstantsMemorySize );
		context->Unmap( _materialConstants, 0 );

		context->IASetVertexBuffers( 0, 1, &_DynamicVertexBuffers[effect.VertexBuffer].buffer, &stride, &offset );
		context->PSSetShader( _materialShaders[effect.Material->Shader], nullptr, 0 );
		context->PSSetConstantBuffers( 1, 1, &_materialConstants );

		// Find the actual srvs to use.
		ID3D11ShaderResourceView **srvs = new ID3D11ShaderResourceView*[effect.Material->TextureCount];
		for ( uint32_t i = 0; i < effect.Material->TextureCount; ++i )
		{
			TextureProxy texture = effect.Material->Textures[i];
			if ( texture.Index == 1 )
			{
				srvs[i] = nullptr;
			}
			else
			{
				switch ( texture.Type )
				{
				case TextureProxy::Type::Regular:
					srvs[i] = _textures[texture.Index];
					break;

				case TextureProxy::Type::Structured:
					srvs[i] = nullptr;
					break;

				case TextureProxy::Type::StructuredDynamic:
					srvs[i] = _dynamicStructuredBuffers[texture.Index].SRV;
					break;

				default:
					srvs[i] = nullptr;
				}
			}
		}

		context->PSSetShaderResources( 0, effect.Material->TextureCount, srvs );

		SAFE_DELETE_ARRAY( srvs );

		context->Draw( effect.VertexCount, 0 );
	}

	context->OMSetBlendState( nullptr, nullptr, ~0U );
	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	context->IASetInputLayout( nullptr );
}

void Graphics::_GenerateGlow()
{
	ID3D11DeviceContext *context = _D3D11->GetDeviceContext();
	//context->IASetInputLayout(nullptr);
	//ID3D11Buffer *buf = nullptr;
	//context->IASetVertexBuffers( 0, 1, nullptr, nullptr, nullptr );
	context->IASetInputLayout( nullptr );

	//ID3D11Buffer *buf = nullptr;
	//context->IASetVertexBuffers( 0, 1, nullptr, nullptr, nullptr );
	context->IASetInputLayout( nullptr );

	D3D11_VIEWPORT oldVp;
	uint32_t numViewports = 1;
	context->RSGetViewports( &numViewports, &oldVp );

	// Render emissive buffer to low resolution render target
	D3D11_VIEWPORT vp = oldVp;
	vp.Width = 256.0f;
	vp.Height = 256.0f;
	context->RSSetViewports( numViewports, &vp );

	context->OMSetRenderTargets( 1, &_glowTempRT1.RTV, nullptr );
	context->VSSetShader( _fullscreenTextureVS, nullptr, 0 );
	context->PSSetShader( _downSamplePS, nullptr, 0 );
	context->PSSetSamplers( 0, 1, &_triLinearSam );
	ID3D11ShaderResourceView *srv = _GBuffer->EmissiveSRV();
	context->PSSetShaderResources( 0, 1, &srv );

	context->Draw( 3, 0 );

	srv = nullptr;
	context->PSSetShaderResources( 0, 1, &srv );

	// Blur the texture
	D3D11_MAPPED_SUBRESOURCE mappedData;
	context->Map( _blurTexelConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData );
	float texelSizes[] = { 1.0f / vp.Width, 1.0f / vp.Height };
	memcpy( mappedData.pData, texelSizes, sizeof( float ) * 2 );
	context->Unmap( _blurTexelConstants, 0 );

	context->OMSetRenderTargets( 1, &_glowTempRT2.RTV, nullptr );
	context->PSSetShader( _separableBlurHorizontal, nullptr, 0 );
	context->PSSetShaderResources( 0, 1, &_glowTempRT1.SRV );
	context->PSSetConstantBuffers( 1, 1, &_blurTexelConstants );

	context->Draw( 3, 0 );

	context->PSSetShaderResources( 0, 1, &srv );

	// Flip
	context->OMSetRenderTargets( 1, &_glowTempRT1.RTV, nullptr );
	context->PSSetShader( _separableBlurVertical, nullptr, 0 );
	context->PSSetShaderResources( 0, 1, &_glowTempRT2.SRV );

	context->Draw( 3, 0 );

	context->PSSetShaderResources( 0, 1, &srv );

	// Reset viewport
	context->RSSetViewports( 1, &oldVp );

	// Max back to emissive buffer
	ID3D11RenderTargetView *rtv = _GBuffer->EmissiveRT();
	context->OMSetRenderTargets( 1, &rtv, nullptr );
	context->PSSetShader( _fullscreenTexturePSMultiChannel, nullptr, 0 );
	context->PSSetShaderResources( 0, 1, &_glowTempRT1.SRV );

	float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 0.0f };

	context->OMSetBlendState( _bsBlendEnabled.BS, blendFactor, ~0U );

	context->Draw( 3, 0 );

	context->OMSetBlendState( _bsBlendDisabled.BS, nullptr, ~0U );
	context->PSSetShaderResources( 0, 1, &srv );

	context->IASetInputLayout( _inputLayout );
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
		offset = min(offset, 1023);
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
		offset = min(offset, 1023);
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
		offset = min(offset, 1023);
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
		offset = min(offset, 1023);
		((AreaRectLight*)mappedData.pData)[offset] = *l;
		offset++;
	}
	deviceContext->Unmap(resource, 0);
	SAFE_RELEASE(resource);
	_areaRectLights.pop_back();

	// Set shader constants (GBuffer, lights, matrices and so forth)
	TiledDeferredConstants constants;
	/*DirectX::XMStoreFloat4x4( &constants.View, XMMatrixTranspose(DirectX::XMLoadFloat4x4( &_renderCamera->viewMatrix ) ) );
	DirectX::XMStoreFloat4x4( &constants.Proj, XMMatrixTranspose(DirectX::XMLoadFloat4x4( &_renderCamera->projectionMatrix ) ) );
	DirectX::XMStoreFloat4x4( &constants.InvView, XMMatrixTranspose( XMMatrixInverse( nullptr, DirectX::XMLoadFloat4x4( &_renderCamera->viewMatrix ) ) ) );
	DirectX::XMStoreFloat4x4( &constants.InvProj, XMMatrixTranspose( XMMatrixInverse( nullptr, DirectX::XMLoadFloat4x4( &_renderCamera->projectionMatrix ) ) ) );
	
	constants.BackbufferWidth = static_cast<float>(o->GetScreenResolutionWidth());
	constants.BackbufferHeight = static_cast<float>(o->GetScreenResolutionHeight());*/
	constants.PointLightCount = min(static_cast<int>(_pointLights.size()), 1024 );
	constants.SpotLightCount = min(static_cast<int>(_spotLights.size()), 1024 );
	constants.CapsuleLightCount = min(static_cast<int>(_capsuleLights.size()), 1024 );
	constants.AreaRectLightCount = min(static_cast<int>(_areaRectLights.size()), 1024);

	deviceContext->Map( _tiledDeferredConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData );
	memcpy( mappedData.pData, &constants, sizeof( TiledDeferredConstants ) );
	deviceContext->Unmap( _tiledDeferredConstants, 0 );

	//Texture2D<float4> gColorMap : register(t0);
	//Texture2D<float4> gNormalMap : register(t1);
	//Texture2D<float4> gEmissiveMap : register(t2);
	//Texture2D<float4> gDepthMap : register(t3);
	//StructuredBuffer<PointLight> gPointLights : register(t4);
	ID3D11ShaderResourceView *srvs[] =
	{
		_GBuffer->ColorSRV(),
		_GBuffer->NormalSRV(),
		_GBuffer->EmissiveSRV(),
		_mainDepth.SRV,
		_GBuffer->LightFinSRV(),
		_GBuffer->LightVolSRV(),
		_pointLightsBuffer.SRV,
		_spotLightsBuffer.SRV,
		_capsuleLightsBuffer.SRV,
		_areaRectLightBuffer.SRV
	};
	auto o = System::GetOptions();
	ID3D11Buffer *buffers[] = { _tiledDeferredConstants };
	deviceContext->VSSetShader( nullptr, nullptr, 0 );
	deviceContext->PSSetShader( nullptr, nullptr, 0 );
	deviceContext->CSSetShader( _tiledDeferredCS, nullptr, 0 );
	deviceContext->CSSetConstantBuffers( 1, 1, buffers );
	deviceContext->CSSetSamplers( 0, 1, &_anisoSam );
	deviceContext->CSSetShaderResources( 0, 10, srvs );
	deviceContext->CSSetUnorderedAccessViews( 0, 1, &_accumulateRT.UAV, nullptr );

	int groupCount[2];
	groupCount[0] = static_cast<uint32_t>(ceil(o->GetScreenResolutionWidth() / 16.0f ));
	groupCount[1] = static_cast<uint32_t>(ceil(o->GetScreenResolutionHeight() / 16.0f ));

	deviceContext->Dispatch( groupCount[0], groupCount[1], 1 );

	// Unbind stuff
	for ( int i = 0; i < 10; ++i )
	{
		srvs[i] = nullptr;
	}
	deviceContext->CSSetShaderResources( 0,10, srvs );
	ID3D11UnorderedAccessView *nullUAV[] = { nullptr };
	deviceContext->CSSetUnorderedAccessViews( 0, 1, nullUAV, nullptr );
	deviceContext->CSSetShader( nullptr, nullptr, 0 );
	deviceContext->CSSetConstantBuffers( 1, 0, nullptr );
	deviceContext->CSSetSamplers( 0, 0, nullptr );
}

void Graphics::_RenderLights()
{
	auto deviceContext = _D3D11->GetDeviceContext();

	float color[] = { 0.0f,0.0f,0.0f,0.0f };

	ID3D11RenderTargetView *rtvs[] = { _GBuffer->LightRT(), _GBuffer->LightFinRT(), _GBuffer->LightVolRT() , _GBuffer->ColorRT(), _GBuffer->NormalRT() };
	ID3D11ShaderResourceView *srvs[] = { _GBuffer->LightSRV(), _GBuffer->DepthSRV(), _GBuffer->LightVolSRV(), nullptr, nullptr, nullptr };
	deviceContext->ClearRenderTargetView(rtvs[0], color);
	deviceContext->ClearRenderTargetView(rtvs[1], color);
	deviceContext->ClearRenderTargetView(rtvs[2], color);
	deviceContext->PSSetSamplers(0, 1, &_triLinearSam);
	deviceContext->OMSetDepthStencilState(_dssWriteToDepthDisabled.DSS, 1);

	uint32_t stride = sizeof(LightGeoLayout);
	uint32_t offset = 0;

	deviceContext->VSSetShader(_lightVertexShader, nullptr, 0);


	float blendFactor[4] = {1.0f, 1.0f, 1.0f, 1.0f };
	float blendFactor2[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
	UINT sampleMask = 0xffffffff;



	// Point light
	deviceContext->IASetVertexBuffers(0, 1, &_PointLightData.vertexbuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(_PointLightData.indexBuffer, DXGI_FORMAT_R32_UINT, 0); //-V108

	deviceContext->OMSetBlendState(_bsBlendEnabled.BS, blendFactor, sampleMask);
	deviceContext->RSSetState(_rsBackFaceCullingEnabled.RS);
	deviceContext->OMSetRenderTargets(1, &rtvs[1], _mainDepth.DSV);
	deviceContext->PSSetShader(_lightFinalPixelShader, nullptr, 0);
	deviceContext->PSSetShaderResources(0, 2, &srvs[0]);

	StaticMeshVSConstants vsConstants;
	ZeroMemory(&vsConstants, sizeof(StaticMeshVSConstants));

	XMMATRIX world, worldView, wvp, worldViewInvTrp, view, viewproj;
	view = DirectX::XMLoadFloat4x4(&_renderCamera->viewMatrix);
	viewproj = DirectX::XMLoadFloat4x4(&_renderCamera->viewProjectionMatrix);
	for (auto p : _pointLights)
	{
		if (p->volumetrick)
		{
			world = DirectX::XMMatrixScalingFromVector(XMVectorSet(p->blobRange, p->blobRange, p->blobRange,0.0f))* DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&p->position));

			worldView = world * view;

			wvp = DirectX::XMMatrixTranspose(world * viewproj);
			worldViewInvTrp = DirectX::XMMatrixInverse(nullptr, worldView); // Normally transposed, but since it's done again for shader I just skip it

																   // Set object specific constants.
			
			DirectX::XMStoreFloat4x4(&vsConstants.WVP, wvp);
			DirectX::XMStoreFloat4x4(&vsConstants.WorldViewInvTrp, worldViewInvTrp);
			DirectX::XMStoreFloat4x4(&vsConstants.World, world);
			DirectX::XMStoreFloat4x4(&vsConstants.WorldView, DirectX::XMMatrixTranspose(worldView));
	
			// Update shader constants.
			D3D11_MAPPED_SUBRESOURCE mappedData;
			deviceContext->Map(_staticMeshVSConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
			memcpy(mappedData.pData, &vsConstants, sizeof(StaticMeshVSConstants));
			deviceContext->Unmap(_staticMeshVSConstants, 0);

			deviceContext->Map(_PointLightData.constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
			memcpy(mappedData.pData, p, sizeof(PointLight));
			deviceContext->Unmap(_PointLightData.constantBuffer, 0);

			ID3D11Buffer* buf[] = { _staticMeshVSConstants, _PointLightData.constantBuffer };

			deviceContext->VSSetConstantBuffers(1, 1, buf);
			deviceContext->PSSetConstantBuffers(1, 1, &buf[1]);


			// Backfaces
			//deviceContext->RSSetState(_rsFrontFaceCullingEnabled.RS);
			//deviceContext->OMSetRenderTargets(1, rtvs, nullptr);//_mainDepth.DSV);
			//deviceContext->PSSetShader(_lightBackFacePixelShader, nullptr, 0);
			//deviceContext->PSSetShaderResources(0, 2, &srvs[2]);
			//deviceContext->OMSetBlendState(_bsBlendDisabled.BS, blendFactor, sampleMask);

			//deviceContext->DrawIndexed(_PointLightData.indexCount, 0, 0);


			// Front faces
	

			

			deviceContext->DrawIndexed(_PointLightData.indexCount, 0, 0);

		}
	}
	// Spot light
	deviceContext->IASetVertexBuffers(0, 1, &_SpotLightData.vertexbuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(_SpotLightData.indexBuffer, DXGI_FORMAT_R32_UINT, 0); //-V108

	for (auto p : _spotLights)
	{
		if (p->volumetrick)
		{
			world = DirectX::XMLoadFloat4x4(&p->world);

			worldView = world * view;

			wvp = DirectX::XMMatrixTranspose(world * viewproj);
			worldViewInvTrp = DirectX::XMMatrixInverse(nullptr, worldView); // Normally transposed, but since it's done again for shader I just skip it

																			// Set object specific constants.

			DirectX::XMStoreFloat4x4(&vsConstants.WVP, wvp);
			DirectX::XMStoreFloat4x4(&vsConstants.WorldViewInvTrp, worldViewInvTrp);
			DirectX::XMStoreFloat4x4(&vsConstants.World, world);
			DirectX::XMStoreFloat4x4(&vsConstants.WorldView, DirectX::XMMatrixTranspose(worldView));

			// Update shader constants.
			D3D11_MAPPED_SUBRESOURCE mappedData;
			deviceContext->Map(_staticMeshVSConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
			memcpy(mappedData.pData, &vsConstants, sizeof(StaticMeshVSConstants));
			deviceContext->Unmap(_staticMeshVSConstants, 0);

			deviceContext->Map(_SpotLightData.constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
			memcpy(mappedData.pData, p, sizeof(SpotLight));
			deviceContext->Unmap(_SpotLightData.constantBuffer, 0);

			ID3D11Buffer* buf[] = { _staticMeshVSConstants, _SpotLightData.constantBuffer };

			deviceContext->VSSetConstantBuffers(1, 1, buf);
			deviceContext->PSSetConstantBuffers(1, 1, &buf[1]);


			// Backfaces
			deviceContext->RSSetState(_rsFrontFaceCullingEnabled.RS);
			deviceContext->PSSetShaderResources(0, 3, &srvs[3]);
			deviceContext->OMSetRenderTargets(1, rtvs, nullptr);//_mainDepth.DSV);
			deviceContext->PSSetShader(_lightBackFacePixelShader, nullptr, 0);
			deviceContext->OMSetBlendState(_bsBlendDisabled.BS, blendFactor2, sampleMask);
			deviceContext->OMSetDepthStencilState(_dssWriteToDepthDisabled.DSS, 1);
			deviceContext->DrawIndexed(_SpotLightData.indexCount, 0, 0);


			// Front faces


			deviceContext->OMSetBlendState(_bsBlendEnabled.BS, blendFactor2, sampleMask);
			deviceContext->RSSetState(_rsBackFaceCullingEnabled.RS);
			deviceContext->OMSetRenderTargets(1, &rtvs[2], _mainDepth.DSV);
			deviceContext->PSSetShaderResources(0, 2, &srvs[0]);
			deviceContext->PSSetShader(_lightFrontFacePixelShader, nullptr, 0);
		//	deviceContext->OMSetDepthStencilState(_dssWriteToDepthEnabled.DSS, 1);

			deviceContext->DrawIndexed(_SpotLightData.indexCount, 0, 0);

		}
	}
	// Unbind stuff
	deviceContext->PSSetShaderResources(0, 3, &srvs[3]);
	for (uint i = 0; i < 5; i++)
	{
		rtvs[i] = nullptr;
	}
	deviceContext->OMSetRenderTargets(5, rtvs, nullptr);
	deviceContext->OMSetBlendState(_bsBlendDisabled.BS, blendFactor, sampleMask);
	deviceContext->OMSetDepthStencilState(_dssWriteToDepthEnabled.DSS, 1);
	deviceContext->RSSetState(_rsBackFaceCullingEnabled.RS);
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
	ID3D11PixelShader *ps = _fullscreenTexturePSMultiChannelGamma;
	deviceContext->VSSetShader(_fullscreenTextureVS, nullptr, 0);
	deviceContext->PSSetShader(ps, nullptr, 0);



	ID3D11ShaderResourceView *nullSRV = nullptr;
	deviceContext->PSSetShaderResources(0, 1, &nullSRV);


	float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
	UINT sampleMask = 0xffffffff;


	deviceContext->OMSetBlendState(_bsBlendEnabled.BS, blendFactor, sampleMask);

	for (auto job : _overlayRenderJobs)
	{
		// Find the actual srvs to use.
		ID3D11ShaderResourceView **srvs = new ID3D11ShaderResourceView*[job->material->TextureCount];
		for (uint32_t i = 0; i < job->material->TextureCount; ++i)
		{
			TextureProxy& texture = job->material->Textures[i];
			if ( texture.Index == 1 )
			{
				srvs[i] = nullptr;
			}
			else
			{
				switch ( texture.Type )
				{
				case TextureProxy::Type::Regular:
					srvs[i] = _textures.find(texture.Index)->second;
					break;

				case TextureProxy::Type::Structured:

					break;

				case TextureProxy::Type::StructuredDynamic:
					srvs[i] = _dynamicStructuredBuffers[texture.Index].SRV;
					break;

				default:
					srvs[i] = nullptr;
				}
			}
		}

		deviceContext->PSSetShaderResources(0, job->material->TextureCount, srvs);

		// Bind the viewport to use
		D3D11_VIEWPORT vp;
		vp.Height = job->height;
		vp.Width = job->width;
		vp.TopLeftX = job->posX;
		vp.TopLeftY = job->posY;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		deviceContext->RSSetViewports(1, &vp);

		deviceContext->Draw(3, 0);

		SAFE_DELETE_ARRAY(srvs);
	}

	deviceContext->OMSetBlendState(_bsBlendDisabled.BS, nullptr, sampleMask);
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
	deviceContext->IASetInputLayout(_textInputLayout);

	// Set constant buffer
//	TextVSConstants vsConstants;
	//DirectX::XMStoreFloat4x4(& vsConstants.Ortho, DirectX::XMMatrixTranspose( DirectX::XMLoadFloat4x4(&_orthoMatrix)));

	/*D3D11_MAPPED_SUBRESOURCE mappedData;
	deviceContext->Map(_textVSConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &vsConstants, sizeof(TextVSConstants));
	deviceContext->Unmap(_textVSConstantBuffer, 0);

	deviceContext->VSSetConstantBuffers(0, 1, &_textVSConstantBuffer);
*/

	// Bind shaders
	deviceContext->VSSetShader(_textVSShader, nullptr, 0);
	deviceContext->PSSetShader(_textPSShader, nullptr, 0);
	ID3D11PixelShader *ps = _fullscreenTexturePSMultiChannelGamma;
	//deviceContext->VSSetShader(_fullscreenTextureVS, nullptr, 0);
	//deviceContext->PSSetShader(ps, nullptr, 0);

	float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
	UINT sampleMask = 0xffffffff;


	deviceContext->OMSetBlendState(_bsBlendEnabled.BS, blendFactor, sampleMask);

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
			deviceContext->IASetVertexBuffers(0, 1, &_DynamicVertexBuffers[j2->VertexBuffer].buffer, &stride, &offset);

			// Set constant buffer
			TextPSConstants psConstants;
			psConstants.Color = j2->Color;

			D3D11_MAPPED_SUBRESOURCE mappedData;
			deviceContext->Map(_textPSConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
			memcpy(mappedData.pData, &psConstants, sizeof(TextPSConstants));
			deviceContext->Unmap(_textPSConstantBuffer, 0);

			deviceContext->PSSetConstantBuffers(1, 1, &_textPSConstantBuffer);

			uint count = 0;
			for (auto& t : j2->text)
				if (!(t == 32 || t == 10))
					count++;

			// Render
			deviceContext->Draw(count*6, 0);
		}
	}

	deviceContext->OMSetBlendState(_bsBlendDisabled.BS, nullptr, sampleMask);
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
			_GBuffer->NormalSRV(),
			_GBuffer->LightSRV(),
			_GBuffer->LightVolSRV(),
			_GBuffer->ColorSRV()// _GBuffer->NormalSRV()
		};

		auto o = System::GetOptions();
		float width = (float)o->GetScreenResolutionWidth();
		float height = (float)o->GetScreenResolutionHeight();
		D3D11_VIEWPORT vp[4];
		for (int i = 0; i < 4; ++i)
		{
			vp[i].MinDepth = 0.0f;
			vp[i].MaxDepth = 1.0f;
			vp[i].Width = width / 2.0f;
			vp[i].Height = height / 2.0f;
			vp[i].TopLeftX = (i % 2) * width / 2.0f;
			vp[i].TopLeftY = (uint32_t)(0.5f * i) * height / 2.0f;
		}

		if (numImages == 1)
		{
			vp[0].Width = static_cast<float>(width);
			vp[0].Height = static_cast<float>(height);
		}

		// Here begins actual render code

		for (uint32_t i = 0; i < numImages; ++i)
		{
			ID3D11PixelShader *ps = _fullscreenTexturePSMultiChannel;
			if (srvs[i] == _mainDepth.SRV || srvs[i] == _GBuffer->LightSRV())
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

const Graphics::SpotLightData Graphics::_CreateSpotLightData(unsigned detail)
{
	PointLightData geo;
	geo.mesh = new Mesh;
	//geo.mesh->GenerateSphere(detail);
	geo.mesh->GenerateCone(5);
	geo.indexCount = geo.mesh->IndexCount();
	LightGeoLayout *completeVertices = new LightGeoLayout[geo.mesh->IndexCount()];

	auto positions = geo.mesh->AttributeData(geo.mesh->FindStream(Mesh::AttributeType::Position));
	auto positionIndices = geo.mesh->AttributeIndices(geo.mesh->FindStream(Mesh::AttributeType::Position));

	auto normals = geo.mesh->AttributeData(geo.mesh->FindStream(Mesh::AttributeType::Normal));
	auto normalIndices = geo.mesh->AttributeIndices(geo.mesh->FindStream(Mesh::AttributeType::Normal));

	for (unsigned i = 0; i < geo.mesh->IndexCount(); ++i)
	{
		completeVertices[i]._position = ((DirectX::XMFLOAT3*)positions.data())[positionIndices[i]];
		completeVertices[i]._normal = ((DirectX::XMFLOAT3*)normals.data())[normalIndices[i]];
	}
	unsigned *completeIndices = new unsigned[geo.mesh->IndexCount()];
	uint vertexDataSize = sizeof(LightGeoLayout) * geo.mesh->IndexCount();

	unsigned counter = 0;
	for (unsigned batch = 0; batch < geo.mesh->BatchCount(); ++batch)
	{
		for (unsigned i = 0; i < geo.mesh->Batches()[batch].IndexCount; ++i)
		{
			completeIndices[counter++] = geo.mesh->Batches()[batch].StartIndex + i; //-V108
		}
	}

	//for (unsigned i = 0; i < geo.mesh->IndexCount(); i += 3)
	//{
	//	swap(completeIndices[i], completeIndices[i + 2]);
	//}
	geo.vertexbuffer = nullptr;
	geo.indexBuffer = nullptr;
	geo.vertexbuffer = _CreateVertexBuffer((void*)completeVertices, vertexDataSize);
	geo.indexBuffer = _CreateIndexBuffer(completeIndices, geo.mesh->IndexCount()*sizeof(unsigned));
	if (!geo.vertexbuffer || !geo.indexBuffer)
	{
		SAFE_DELETE_ARRAY(completeVertices);
		SAFE_DELETE_ARRAY(completeIndices);
		SAFE_RELEASE(geo.vertexbuffer);
		SAFE_RELEASE(geo.indexBuffer);
		throw ErrorMsg(5000044, L"Failed to create spot light geo buffers.");
	}

	SAFE_DELETE_ARRAY(completeVertices);
	SAFE_DELETE_ARRAY(completeIndices);

	auto device = _D3D11->GetDevice();
	D3D11_BUFFER_DESC bufDesc;
	memset(&bufDesc, 0, sizeof(D3D11_BUFFER_DESC));
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufDesc.ByteWidth = sizeof(SpotLight);
	geo.constantBuffer = nullptr;
	HRESULT hr = device->CreateBuffer(&bufDesc, nullptr, &geo.constantBuffer);
	if (FAILED(hr))
	{
		SAFE_RELEASE(geo.vertexbuffer);
		SAFE_RELEASE(geo.indexBuffer);
		SAFE_RELEASE(geo.constantBuffer);
		throw ErrorMsg(5000045, L"Failed to create spot light constant buffer.");
	}

	return geo;
}

const Graphics::PointLightData Graphics::_CreatePointLightData(unsigned detail)
{
	PointLightData geo;
	geo.mesh = new Mesh;
	geo.mesh->GenerateSphere(detail);
	//geo.mesh->GenerateCone(5);
	geo.indexCount = geo.mesh->IndexCount();
	LightGeoLayout *completeVertices = new LightGeoLayout[geo.mesh->IndexCount()];

	auto positions = geo.mesh->AttributeData(geo.mesh->FindStream(Mesh::AttributeType::Position));
	auto positionIndices = geo.mesh->AttributeIndices(geo.mesh->FindStream(Mesh::AttributeType::Position));

	auto normals = geo.mesh->AttributeData(geo.mesh->FindStream(Mesh::AttributeType::Normal));
	auto normalIndices = geo.mesh->AttributeIndices(geo.mesh->FindStream(Mesh::AttributeType::Normal));

	for (unsigned i = 0; i < geo.mesh->IndexCount(); ++i)
	{
		completeVertices[i]._position = ((DirectX::XMFLOAT3*)positions.data())[positionIndices[i]];
		completeVertices[i]._normal = ((DirectX::XMFLOAT3*)normals.data())[normalIndices[i]];
	}
	unsigned *completeIndices = new unsigned[geo.mesh->IndexCount()];
	uint vertexDataSize = sizeof(LightGeoLayout) * geo.mesh->IndexCount();

	unsigned counter = 0;
	for (unsigned batch = 0; batch < geo.mesh->BatchCount(); ++batch)
	{
		for (unsigned i = 0; i < geo.mesh->Batches()[batch].IndexCount; ++i)
		{
			completeIndices[counter++] = geo.mesh->Batches()[batch].StartIndex + i; //-V108
		}
	}

	//for (unsigned i = 0; i < geo.mesh->IndexCount(); i += 3)
	//{
	//	swap(completeIndices[i], completeIndices[i + 2]);
	//}

	geo.vertexbuffer = nullptr;
	geo.indexBuffer = nullptr;
	geo.vertexbuffer = _CreateVertexBuffer((void*)completeVertices, vertexDataSize);
	geo.indexBuffer = _CreateIndexBuffer(completeIndices, geo.mesh->IndexCount()*sizeof(unsigned));
	if (!geo.vertexbuffer || !geo.indexBuffer)
	{
		SAFE_DELETE_ARRAY(completeVertices);
		SAFE_DELETE_ARRAY(completeIndices);
		SAFE_RELEASE(geo.vertexbuffer);
		SAFE_RELEASE(geo.indexBuffer);
		throw ErrorMsg(5000038, L"Failed to create point light geo buffers.");
	}

	SAFE_DELETE_ARRAY(completeVertices);
	SAFE_DELETE_ARRAY(completeIndices);

	auto device = _D3D11->GetDevice();
	D3D11_BUFFER_DESC bufDesc;
	memset(&bufDesc, 0, sizeof(D3D11_BUFFER_DESC));
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufDesc.ByteWidth = sizeof(PointLight);
	geo.constantBuffer = nullptr;
	HRESULT hr = device->CreateBuffer(&bufDesc, nullptr, &geo.constantBuffer);
	if (FAILED(hr))
	{
		SAFE_RELEASE(geo.vertexbuffer);
		SAFE_RELEASE(geo.indexBuffer);
		SAFE_RELEASE(geo.constantBuffer);
		throw ErrorMsg(5000039, L"Failed to create point light constant buffer.");
	}

	return geo;
}

const void Graphics::_DeletePointLightData(PointLightData & geo) const
{
	SAFE_DELETE(geo.mesh);
	SAFE_RELEASE(geo.vertexbuffer);
	SAFE_RELEASE(geo.indexBuffer);
	SAFE_RELEASE(geo.constantBuffer);
	return void();
}

const void Graphics::_DeleteSpotLightData(SpotLightData & geo) const
{
	SAFE_DELETE(geo.mesh);
	SAFE_RELEASE(geo.vertexbuffer);
	SAFE_RELEASE(geo.indexBuffer);
	SAFE_RELEASE(geo.constantBuffer);
	return void();
}

Graphics::DecalData Graphics::_CreateDecalData()
{
	DecalData d;
	d.mesh = new Mesh;
	d.mesh->GenerateCube();
	d.indexCount = d.mesh->IndexCount();

	DecalLayout *completeVertices = new DecalLayout[d.mesh->IndexCount()];

	auto positions = d.mesh->AttributeData(d.mesh->FindStream(Mesh::AttributeType::Position));
	auto positionIndices = d.mesh->AttributeIndices(d.mesh->FindStream(Mesh::AttributeType::Position));
	

	for (unsigned int i = 0; i < d.mesh->IndexCount(); ++i)
	{
		completeVertices[i]._position = ((XMFLOAT3*)positions.data())[positionIndices[i]];
	}
	unsigned int* completeIndices = new unsigned int[d.mesh->IndexCount()];
	uint32_t vertexDataSize = sizeof(DecalLayout) * d.mesh->IndexCount();

	unsigned int counter = 0;
	for (unsigned batch = 0; batch < d.mesh->BatchCount(); ++batch)
	{
		for (unsigned i = 0; i < d.mesh->Batches()[batch].IndexCount; ++i)
		{
			completeIndices[counter++] = d.mesh->Batches()[batch].StartIndex + i;
		}
	}

	d.vertexbuffer = nullptr;
	d.indexBuffer = nullptr;
	d.vertexbuffer = _CreateVertexBuffer((void*)completeVertices, vertexDataSize);
	d.indexBuffer = _CreateIndexBuffer(completeIndices, d.mesh->IndexCount()*sizeof(unsigned));
	if (!d.vertexbuffer || !d.indexBuffer)
	{
		SAFE_DELETE_ARRAY(completeVertices);
		SAFE_DELETE_ARRAY(completeIndices);
		SAFE_RELEASE(d.vertexbuffer);
		SAFE_RELEASE(d.indexBuffer);
		throw ErrorMsg(5000039, L"Failed to create Decal vertex- and index buffers.");
	}

	SAFE_DELETE_ARRAY(completeVertices);
	SAFE_DELETE_ARRAY(completeIndices);

	auto device = _D3D11->GetDevice();
	D3D11_BUFFER_DESC bufDesc;
	memset(&bufDesc, 0, sizeof(D3D11_BUFFER_DESC));
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufDesc.ByteWidth = sizeof(DecalsPerObjectBuffer);
	d.constantBuffer = nullptr;
	HRESULT hr = device->CreateBuffer(&bufDesc, nullptr, &d.constantBuffer);
	if (FAILED(hr))
	{
		SAFE_RELEASE(d.vertexbuffer);
		SAFE_RELEASE(d.indexBuffer);
		SAFE_RELEASE(d.constantBuffer);
		throw ErrorMsg(5000041, L"Failed to create Decal constant buffer.");
	}

	return d;
}

void Graphics::_DeleteDecalData(DecalData & dd)
{
	SAFE_DELETE(dd.mesh);
	SAFE_RELEASE(dd.vertexbuffer);
	SAFE_RELEASE(dd.indexBuffer);
	SAFE_RELEASE(dd.constantBuffer);
}

const uint32_t Graphics::_GetNextPrime(uint8_t line)
{
	const static  uint32_t primes[] = { 2 , 3   ,      5   ,      7   ,     11  ,      13  ,      17   ,     19,

		23  ,      29   ,     31  ,      37   ,     41   ,     43   ,     47   ,     53,

		59    ,    61     ,   67      ,  71     ,   73    ,    79    ,    83  ,      89,

		97   ,    101  ,     103   ,    107   ,    109      , 113    ,   127   ,    131,

		137    ,   139      , 149     ,  151      , 157      , 163     ,  167  ,     173,

		179 ,      181      , 191     ,  193     ,  197   ,    199   ,    211    ,   223,

		227    ,   229    ,   233   ,    239   ,    241     ,  251   ,    257  ,     263,

		269  ,     271    ,   277,       281,283,293,307,311,

		313    ,   317,       331 ,      337,347,349,353,359,
		
		367   ,    373,       379  ,     383,389,397,401,409,

		419   ,    421 ,      431   ,    433,439,443,449,457,

		461    ,   463  ,     467    ,   479,487,491,499,503,

		509     ,  521   ,    523     ,  541,547,557,563,569,

		571      , 577    ,   587      , 593,599,601,607,613,

		617       ,619     ,  631       ,641,643,647,653,659,
		
		661      , 673,       677,       683,691,701,709,719,

		727       ,733 ,      739 ,      743,751,757,761,769,

		773,       787  ,     797  ,     809,811,821,823,827,

		829 ,      839   ,    853   ,    857,859,863,877,881,

		883  ,     887    ,   907    ,   911,919,929,937,941,

		947   ,    953     ,  967     ,  971,977,983,991,997,

		1009   ,   1013     , 1019     , 1021,1031,1033,1039,1049,

		1051    ,  1061      ,1063      ,1069,1087,1091,1093,1097,

		1103     , 1109,      1117,      1123,1129,1151,1153,1163,

		1171      ,1181 ,     1187 ,     1193,1201,1213,1217,1223,

		1229,      1231  ,    1237  ,    1249,1259,1277,1279,1283,

		1289 ,     1291   ,   1297   ,   1301,1303,1307,1319,1321,

		1327  ,    1361    ,  1367,      1373,1381,1399,1409,1423,
		
		1427   ,   1429,      1433 ,     1439,1447,1451,1453,1459,

		1471    ,  1481 ,     1483  ,    1487,1489,1493,1499,1511,

		1523     , 1531  ,    1543   ,   1549,1553,1559,1567,1571,

		1579      ,1583   ,   1597    ,  1601,1607,1609,1613,1619,
		
		1621,      1627    ,  1637     , 1657,1663,1667,1669,1693,

		1697 ,     1699     , 1709      ,1721,1723,1733,1741,1747,

		1753  ,    1759      ,1777,      1783,1787,1789,1801,1811,

		1823   ,   1831,      1847 ,     1861,1867,1871,1873,1877,

		1879    ,  1889 ,     1901  ,    1907,1913,1931,1933,1949,

		1951     , 1973  ,    1979   ,   1987,1993,1997,1999,2003,
		
		2011      ,2017   ,   2027,      2029,2039,2053,2063,2069,

		2081,      2083    ,  2087 ,     2089,2099,2111,2113,2129,

		2131 ,     2137     , 2141  ,    2143,2153,2161,2179,2203,

		2207  ,    2213      ,2221   ,   2237,2239,2243,2251,2267,

		2269   ,   2273,      2281    ,  2287,2293,2297,2309,2311,

		2333    ,  2339 ,     2341     , 2347,2351,2357,2371,2377,

		2381     , 2383  ,    2389      ,2393,2399,2411,2417,2423,
		
		2437 ,     2441   ,   2447,      2459,2467,2473,2477,2503 };
	auto& find = _usedPrimes.find(line);
	if (find == _usedPrimes.end())
		_usedPrimes[line] = 0;

	_usedPrimes[line]++;
	return primes[_usedPrimes[line] -1];
}

TextureProxy Graphics::CreateTexture( const wchar_t *filename )
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
		throw ErrorMsg(5000031, L"Texture not found", filename);
	}
	uint32_t id = _GetNextPrime(0);
	_textures[id] = srv;

	return TextureProxy( TextureProxy::Type::Regular, id );
}

const uint64_t Graphics::CreateTextureWrapper(const std::vector<TextureProxy>& textures)
{
	uint64_t newID = 1;
	for (auto& tid : textures)
	{
		newID *= tid.Index;
	}
	auto& find = _texWrappers.find(newID);
	if (find == _texWrappers.end())
		_texWrappers[newID] = std::move(textures);
			
	return newID;
	
}

ID3D11Device * Graphics::GetDevice() const
{
	return _D3D11->GetDevice();
}

ID3D11DeviceContext * Graphics::GetDeviceContext() const
{
	return _D3D11->GetDeviceContext();
}

std::string Graphics::GetAVGTPFTimes()
{
	std::string out = "Gather Total: " + to_string(ctimer.GetAVGTPF("GatherTot"));
	out += "\nMesh: " + to_string(ctimer.GetAVGTPF("Mesh"));
	out += "\nLights: " + to_string(ctimer.GetAVGTPF("Lights"));
	out += "\nCamera: " + to_string(ctimer.GetAVGTPF("Camera"));
	out += "\nOverlay: " + to_string(ctimer.GetAVGTPF("Overlay"));
	out += "\nTexts: " + to_string(ctimer.GetAVGTPF("Texts"));
	out += "\nDecals: " + to_string(ctimer.GetAVGTPF("Decals"));
	out += "\n\nGPU";
	out += "\nRender: " + to_string(timer.GetAVGTPF("Render"));
	out += "\nLights: " + to_string(timer.GetAVGTPF("Lights"));
	out += "\nTiled deferred: " + to_string(timer.GetAVGTPF("Tiled deferred"));
	out += "\nThing: " + to_string(timer.GetAVGTPF("Thing"));
	out += "\nOverlays: " + to_string(timer.GetAVGTPF("Overlays"));
	out += "\nText: " + to_string(timer.GetAVGTPF("Text"));
	out += "\nDecals: " + to_string(timer.GetAVGTPF("Decals"));
	return out;
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

bool Graphics::_BuildTextInputLayout(void)
{

	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout.
	if (FAILED(_D3D11->GetDevice()->CreateInputLayout(vertexDesc, 2, _textShaderInput->GetBufferPointer(), _textShaderInput->GetBufferSize(), &_textInputLayout)))
		return false;

	return true;
}

bool Graphics::_BuildLightInputLayout(void)
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout.
	HRESULT hr = _D3D11->GetDevice()->CreateInputLayout(vertexDesc, 2, _lightShaderInput->GetBufferPointer(), _lightShaderInput->GetBufferSize(), &_lightInputLayout);
	if (FAILED(hr))
		return false;

	// Create the vertex input layout.
	vertexDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	// Create the input layout.
	if ( FAILED( _D3D11->GetDevice()->CreateInputLayout( vertexDesc, 1, _effectVSByteCode->GetBufferPointer(), _effectVSByteCode->GetBufferSize(), &_effectInputLayout ) ) )
		return false;

	return true;
}

bool Graphics::_BuildDecalInputLayout(void)
{
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	HRESULT hr = _D3D11->GetDevice()->CreateInputLayout(vertexDesc, 1, _decalShaderInput->GetBufferPointer(), _decalShaderInput->GetBufferSize(), &_decalsInputLayout);
	if (FAILED(hr))
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

void Graphics::AddEffectProvider( IEffectProvider *provider )
{
	_effectProviders.push_back( provider );
}

void Graphics::AddDecalProvider(IDecalProvider * provider)
{
	_decalProviders.push_back(provider);
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

void Graphics::ClearEffectProviders()
{
	_effectProviders.clear();
}

const void Graphics::ClearDecalProviders()
{
	_decalProviders.clear();
	return void();
}

void Graphics::ReleaseMeshBuffer(uint32_t buffer)
{
	if (buffer >= _MeshBuffers.size())
	{
		TraceDebug("Tried to release nonexistent vertex buffer.");
		return;
	}
	ReleaseVertexBuffer(_MeshBuffers[buffer].VertexBuffer);
	ReleaseIndexBuffer(_MeshBuffers[buffer].IndexBuffer);
}

//Ideally, every manager should have its own vector of vertex and index buffers so it
//can actually delete an entry from the vector without affecting any other components that
//have vertex/index buffers. As it is now, the _VertexBuffer and _IndexBuffer will keep
//growing (with a bunch of nullptrs where it previously had buffers) the more buffer that are
//created. Say you create 100 vertex buffers and delete 100 vertex buffers, when the next one is inserted it
//will sit at index 99 in the _VertexBuffer.
void Graphics::ReleaseVertexBuffer(uint32_t vertexBufferIndex)
{
	if (vertexBufferIndex >= _VertexBuffers.size())
	{
		TraceDebug("Tried to release nonexistent vertex buffer.");
		return;
	}

	SAFE_RELEASE(_VertexBuffers[vertexBufferIndex]);
	//_VertexBuffers.erase(_VertexBuffers.begin() + vertexBufferIndex);
}

void Graphics::ReleaseIndexBuffer(uint32_t indexBufferIndex)
{
	if (indexBufferIndex >= _IndexBuffers.size())
	{
		TraceDebug("Tried to release nonexistent index buffer.");
		return;
	}

	SAFE_RELEASE(_IndexBuffers[indexBufferIndex]);
	//_IndexBuffers.erase(_IndexBuffers.begin() + indexBufferIndex);
}

void Graphics::ReleaseStaticMeshBuffers(const std::vector<uint32_t>& buffers)
{
	std::vector<uint32_t> vind;
	vind.reserve(_MeshBuffers.size());
	std::vector<uint32_t> iind;
	iind.reserve(_MeshBuffers.size());

	for (auto b : buffers)
	{
		vind.push_back(_MeshBuffers[b].VertexBuffer);
		iind.push_back(_MeshBuffers[b].IndexBuffer);
	}
	ReleaseStaticMeshBuffers(vind, iind);
}

void Graphics::ReleaseStaticMeshBuffers(const std::vector<uint32_t>& vbIndices, const std::vector<uint32_t>& ibIndices)
{
	for (auto &i : vbIndices)
		SAFE_RELEASE(_VertexBuffers[i]);
	for (auto &i : ibIndices)
		SAFE_RELEASE(_IndexBuffers[i]);
}

const void Graphics::ReleaseTexture(const TextureProxy& texture)
{
	switch ( texture.Type )
	{
	case TextureProxy::Type::Regular:
		if ( texture.Index >= _textures.size() )
		{
			TraceDebug( "Tried to release nonexistent texture." );
			return;
		}
		SAFE_RELEASE( _textures[texture.Index] );
		break;

	case TextureProxy::Type::Structured:

		break;

	case TextureProxy::Type::StructuredDynamic:
		if ( texture.Index >= _dynamicStructuredBuffers.size() )
		{
			TraceDebug( "Tried to release nonexistant buffer." );
			return;
		}
		_D3D11->DeleteStructuredBuffer( _dynamicStructuredBuffers[texture.Index] );
		break;
	}
}

const void Graphics::ReleaseDynamicVertexBuffer(uint buffer)
{
	if (buffer >= _DynamicVertexBuffers.size())
	{
		TraceDebug("Tried to release nonexistent dynamic vertex buffer.");
		return;
	}

	_DeleteDynamicVertexBuffer(_DynamicVertexBuffers[buffer]);
	return void();
}

bool Graphics::CreateMeshBuffers(Mesh * mesh, std::uint32_t & BufferIndex)
{
	uint32_t vb = 0;
	uint32_t ib = 0;
	if (!_CreateMeshBuffers(mesh, vb, ib))
		return false;
	_MeshBuffers.push_back(VertexIndexBuffers(vb, ib));

	BufferIndex = static_cast<unsigned int>(_MeshBuffers.size() - 1);

	return true;
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
	auto o = System::GetOptions();
	if (!_D3D11->Start(h->GetHWnd(), o->GetScreenResolutionWidth(), o->GetScreenResolutionHeight() ))
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
