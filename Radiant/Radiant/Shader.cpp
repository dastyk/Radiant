#include "Shader.h"

#include <d3dcompiler.h>
#include <codecvt>
#include "Utils.h"
#include "System.h"

#pragma comment(lib, "D3DCompiler.lib")

ID3D10Blob *CompileShader(
	const wchar_t *filename,
	const char *entry,
	const char *profile,
	const D3D_SHADER_MACRO *defines,
	ID3DInclude *includes )
{
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3D10Blob *compiledShader;
	ID3D10Blob *errorMsgs;

	HRESULT hr = D3DCompileFromFile(
		filename,
		defines,
		includes,
		entry,
		profile,
		shaderFlags,
		0,					// flags for compiling effect, not shader. Not used for shader.
		&compiledShader,
		&errorMsgs );

	if ( FAILED( hr ) )
	{
		if ( errorMsgs )
		{
			std::string message = "Error compiling '";
			message += std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes( filename );
			message += "': ";
			message += reinterpret_cast<char*>(errorMsgs->GetBufferPointer());

			TraceDebug( message.c_str() );

			SAFE_RELEASE( errorMsgs );
			throw ErrorMsg(5000024, S2WS(message));
		}

		return nullptr;
	}

	return compiledShader;
}

ID3D10Blob *CompileShader(
	const void *data,
	int bytes,
	const char *entry,
	const char *profile,
	const D3D_SHADER_MACRO *defines,
	ID3DInclude *include )
{
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3D10Blob *compiledShader;
	ID3D10Blob *errorMsgs;

	HRESULT hr = D3DCompile(
		data,
		bytes,
		nullptr, // can be used for strings that specify error messages
		defines,
		include,
		entry,
		profile,
		shaderFlags,
		0, // flags for compiling effect, not shader. Ignored when compiling shader.
		&compiledShader,
		&errorMsgs
		);

	if ( FAILED( hr ) )
	{
		if ( errorMsgs )
		{
			std::string message = "Error compiling shader: ";
			message += reinterpret_cast<char*>(errorMsgs->GetBufferPointer());
			TraceDebug( message.c_str() );
			SAFE_RELEASE( errorMsgs );
			throw ErrorMsg(5000025, S2WS(message));
		}

		return nullptr;
	}

	return compiledShader;
}

ID3D11VertexShader *CompileVSFromFile(
	ID3D11Device *device,
	const wchar_t *filename,
	const char *entry,
	const char *profile,
	const D3D_SHADER_MACRO *defines,
	ID3DInclude *includes,
	ID3D10Blob **byteCode )
{
	ID3D10Blob *compiledShader = CompileShader( filename, entry, profile, defines, includes );
	if ( !compiledShader )
		return nullptr;

	// Create the actual vertex shader.
	ID3D11VertexShader *vertexShader = nullptr;
	if ( FAILED( device->CreateVertexShader( compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, &vertexShader ) ) )
	{
		std::string message = "Failed to create a vertex shader of '";
		message += std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes( filename );
		message += "'";

		TraceDebug( message.c_str() );

		SAFE_RELEASE( compiledShader );
		throw ErrorMsg(5000026, S2WS(message));
		return nullptr;
	}

	// If the user wants the compiled shader byte code we return it and let the user
	// release the memory when done with it. Otherwise we do it right here.
	if ( byteCode )
		*byteCode = compiledShader;
	else
		SAFE_RELEASE( compiledShader );

	return vertexShader;
}

ID3D11GeometryShader *CompileGSFromFile(
	ID3D11Device *device,
	const wchar_t *filename,
	const char *entry,
	const char *profile,
	const D3D_SHADER_MACRO *defines,
	ID3DInclude *includes,
	ID3D10Blob **byteCode )
{
	ID3D10Blob *compiledShader = CompileShader( filename, entry, profile, defines, includes );
	if ( !compiledShader )
		return nullptr;

	// Create the actual geometry shader.
	ID3D11GeometryShader *geometryShader = nullptr;
	if ( FAILED( device->CreateGeometryShader( compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, &geometryShader ) ) )
	{
		std::string message = "Failed to create a geometry shader of '";
		message += std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes( filename );
		message += "'";

		TraceDebug( message.c_str() );

		SAFE_RELEASE( compiledShader );
		throw ErrorMsg(5000027, S2WS(message));
		return nullptr;
	}

	// If the user wants the compiled shader byte code we return it and let the user
	// release the memory when done with it. Otherwise we do it right here.
	if ( byteCode )
		*byteCode = compiledShader;
	else
		SAFE_RELEASE( compiledShader );

	return geometryShader;
}

ID3D11PixelShader *CompilePSFromFile(
	ID3D11Device *device,
	const wchar_t *filename,
	const char *entry,
	const char *profile,
	const D3D_SHADER_MACRO *defines,
	ID3DInclude *includes,
	ID3D10Blob **byteCode )
{
	ID3D10Blob *compiledShader = CompileShader( filename, entry, profile, defines, includes );
	if ( !compiledShader )
		return nullptr;

	// Create the actual pixel shader.
	ID3D11PixelShader *pixelShader = nullptr;
	if ( FAILED( device->CreatePixelShader( compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, &pixelShader ) ) )
	{
		std::string message = "Failed to create a pixel shader of '";
		message += std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes( filename );
		message += "'";

		TraceDebug( message.c_str() );

		SAFE_RELEASE( compiledShader );


		throw ErrorMsg(5000028, S2WS(message));
		return nullptr;
	}

	// If the user wants the compiled shader byte code we return it and let the user
	// release the memory when done with it. Otherwise we do it right here.
	if ( byteCode )
		*byteCode = compiledShader;
	else
		SAFE_RELEASE( compiledShader );

	return pixelShader;
}

ID3D11ComputeShader *CompileCSFromFile(
	ID3D11Device *device,
	const wchar_t *filename,
	const char *entry,
	const char *profile,
	const D3D_SHADER_MACRO *defines,
	ID3DInclude *includes,
	ID3D10Blob **byteCode )
{
	ID3D10Blob *compiledShader = CompileShader( filename, entry, profile, defines, includes );
	if ( !compiledShader )
		return nullptr;

	// Create the actual compute shader.
	ID3D11ComputeShader *computeShader = nullptr;
	if ( FAILED( device->CreateComputeShader( compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, &computeShader ) ) )
	{
		std::string message = "Failed to create a compute shader of '";
		message += std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes( filename );
		message += "'";

		TraceDebug( message.c_str() );

		SAFE_RELEASE( compiledShader );
		throw ErrorMsg(5000029, S2WS(message));
		return nullptr;
	}

	// If the user wants the compiled shader byte code we return it and let the user
	// release the memory when done with it. Otherwise we do it right here.
	if ( byteCode )
		*byteCode = compiledShader;
	else
		SAFE_RELEASE( compiledShader );

	return computeShader;
}