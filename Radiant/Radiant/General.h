#pragma once

//////////////
// Includes //
//////////////
#include <string>
#include <exception>


////////////////////
// Local Includes //
////////////////////
#include "State.h"

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
struct FinishMsg
{
	uint finishMsg;
	std::wstring text;

	FinishMsg(uint fM, std::wstring t) : finishMsg(fM), text(t)
	{};

	FinishMsg(uint fM) : finishMsg(fM), text(L"")
	{};
};

struct StateChange
{
	State* state;
	bool clearPrevious;

	StateChange(State* pS, bool clearPrev) : state(pS), clearPrevious(clearPrev)
	{ };
	StateChange(State* pS) : state(pS), clearPrevious(true)
	{ };
};

// Macros

/// Keys
#define NROFKEYS 256

#define VK_W 0x57
#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44
#define VK_C 0x43
#define VK_F 0x46
#define VK_L 0x4C

#define NROFMOUSEKEYS 3
#define LMOUSE 0
#define RMOUSE 1
#define MMOUSE 2