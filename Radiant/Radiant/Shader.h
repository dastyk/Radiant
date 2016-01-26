#ifndef _SHADER_H_
#define _SHADER_H_

#include <d3d11.h>
#include <d3d11shader.h>

// Returns compiled shader (nullptr if failed)
ID3D10Blob *CompileShader(
	const wchar_t *filename,
	const char *entry,
	const char *profile,
	const D3D_SHADER_MACRO *defines = nullptr,
	ID3DInclude *includes = nullptr );

// Returns compiled shader (nullptr if failed). This variant gets data to compile.
ID3D10Blob *CompileShader(
	const void *data,
	int bytes,
	const char *entry,
	const char *profile,
	const D3D_SHADER_MACRO *defines = nullptr,
	ID3DInclude *include = nullptr );

// Returns nullptr on failure. Optionally returns shader byte code. Note that it's the caller's
// responsibility to release the shader (and byte code, if requested).
ID3D11VertexShader *CompileVSFromFile(
	ID3D11Device *device,
	const wchar_t *filename,
	const char *entry,
	const char *profile,
	const D3D_SHADER_MACRO *defines = nullptr,
	ID3DInclude *includes = nullptr,
	ID3D10Blob **byteCode = nullptr );

ID3D11GeometryShader *CompileGSFromFile(
	ID3D11Device *device,
	const wchar_t *filename,
	const char *entry,
	const char *profile,
	const D3D_SHADER_MACRO *defines = nullptr,
	ID3DInclude *includes = nullptr,
	ID3D10Blob **byteCode = nullptr );

ID3D11PixelShader *CompilePSFromFile(
	ID3D11Device *device,
	const wchar_t *filename,
	const char *entry,
	const char *profile,
	const D3D_SHADER_MACRO *defines = nullptr,
	ID3DInclude *includes = nullptr,
	ID3D10Blob **byteCode = nullptr );

ID3D11ComputeShader *CompileCSFromFile(
	ID3D11Device *device,
	const wchar_t *filename,
	const char *entry,
	const char *profile,
	const D3D_SHADER_MACRO *defines = nullptr,
	ID3DInclude *includes = nullptr,
	ID3D10Blob **byteCode = nullptr );

#endif // _SHADER_H_