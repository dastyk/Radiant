#pragma once

#include <Windows.h>
#include "General.h"

#if defined(DEBUG) || defined(_DEBUG)

#ifndef TraceDebug
#define TraceDebug(message, ...) Utils::OutputDebugTrace(__FILE__, __FUNCTION__, __LINE__, message, __VA_ARGS__)
#endif

#ifndef TraceHR
#define TraceHR(hr) Utils::OutputHRTrace( __FILE__, __FUNCTION__, __LINE__, hr )
#endif

#ifndef HR
#define HR(x) { HRESULT hr = (x); if ( FAILED(hr) ) { Utils::OutputHRTrace( __FILE__, __FUNCTION__, __LINE__, hr ); } }
#endif

#ifndef HR_RETURN
#define HR_RETURN(x) { HRESULT hr = (x); if ( FAILED(hr) ) { return Utils::OutputHRTrace( __FILE__, __FUNCTION__, __LINE__, hr ); } }
#endif

#else

#ifndef TraceDebug
#define TraceDebug(message, ...)
#endif

#ifndef TraceHR
#define TraceHR(hr)
#endif

#ifndef HR
#define HR(x) { HRESULT hr = (x); }
#endif

#ifndef HR_RETURN
#define HR_RETURN(x) { HRESULT hr = (x); if( FAILED(hr) ) { return hr; } }
#endif

#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) { if (x) { delete (x); (x) = nullptr; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) { if (x) { delete[] (x); (x) = nullptr; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) { if (x) { (x)->Release(); (x) = nullptr; } }
#endif

// I would prefer to have logging functionality in it's own specific Log class
// that can output to a console window for instance.
class Utils
{
public:
	static void OutputDebugTrace(const char *file, const char *function, const unsigned long line, const char *message, ...);
	static HRESULT OutputHRTrace(const char *file, const char *function, const unsigned long line, HRESULT hr);
};