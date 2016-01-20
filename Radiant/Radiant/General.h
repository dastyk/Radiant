#pragma once

//////////////
// Includes //
//////////////
#include <string>
#include <exception>

//////////////
// Defines	//
//////////////


// Typedef
typedef unsigned int uint;
typedef unsigned __int8 uint8;
typedef unsigned __int16 uint16;

struct ErrorMsg
{
	uint errorMsg;
	std::wstring errorText;

	ErrorMsg(uint eM, std::wstring eT) : errorMsg(eM), errorText(eT)
	{};

	ErrorMsg(uint eM) : errorMsg(eM), errorText(L"")
	{};
};

// Macros

/// Buttons
#define LMOUSE 0
#define RMOUSE 1
#define MMOUSE 2