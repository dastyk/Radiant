#include "Utils.h"
#include <stdio.h>
#include "General.h"
void Utils::OutputDebugTrace(const char *file, const char *function, const unsigned long line, const char *message, ...)
{
	// Find the pointer to the last occurence of \. Increment by one because we don't want the actual \.
	const char *fileName = strrchr(file, '\\') + 1;

	const char *infoFormat = "[%s in %s at line %lu]: ";

	// Get the number of characters of the formatted string.
	// _scprintf doesn't count the null terminator, which would normally be required.
	// Since we are going to concatenate the string it's not needed right now.
	int lenInfo = _scprintf(infoFormat, function, fileName, line);

	// Get the optional arguments of this function.
	va_list argPtr;
	va_start(argPtr, message);

	// Get how a long a formatted string using the optional arguments would be.
	// This function doesn't count null termination, so we add it.
	int len = lenInfo + _vscprintf(message, argPtr) + 1;

	// One more for \n.
	len++;

	// Allocate memory for the output message.
	char *output = new char[len];

	// Write info string and after that the message followed by a newline character.
	sprintf_s(output, len, infoFormat, function, fileName, line);
	vsprintf_s(output + lenInfo, len - lenInfo, message, argPtr);
	strcat_s(output, len, "\n");
	std::string s = output;
	OutputDebugStringA(output);
	delete[] output;
	//throw ErrorMsg(999999999, S2WS(s));
}

HRESULT Utils::OutputHRTrace(const char *file, const char *function, const unsigned long line, HRESULT hr)
{
	/*
	From http://blogs.msdn.com/b/chuckw/archive/2012/04/24/where-s-dxerr-lib.aspx :
	"For the Windows SDK 8.0 this library was not brought forward (see "Where is the DirectX SDK?").
	This is primarily because HRESULTS for DirectX graphics APIs were added to the FormatMessage
	function when using FORMAT_MESSAGE_FROM_SYSTEM in Windows 8 which already supports most of the
	system error codes reported by DXERR."
	*/

	char *output = nullptr;
	DWORD result = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), output, 0, nullptr);

	if (result == 0)
	{
		output = "Unknown message.";
	}

	OutputDebugTrace(file, function, line, output);

	// Alternative:
	//#include <comdef.h>
	//_com_error err( hr );
	//OutputDebugString( err.ErrorMessage() );

	return hr;
}

std::wstring Utils::s2ws(const std::string & s)

{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}
