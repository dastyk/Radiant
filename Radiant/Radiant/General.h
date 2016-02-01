#ifndef _GENERAL_H_
#define _GENERAL_H_
#pragma once
//////////////
// Includes //
//////////////
#include <string>
#include <exception>
#include <Windows.h>
#include <stdlib.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

////////////////////
// Local Includes //
////////////////////
#include "State.h"

//////////////
// Defines	//
//////////////

// Typedef
typedef std::uint32_t uint;
typedef unsigned __int8 uint8;
typedef unsigned __int16 uint16;

struct ErrorMsg
{
	uint errorMsg;
	std::wstring errorText;
	std::wstring caption;

	ErrorMsg(uint eM, std::wstring eT, std::wstring c) : errorMsg(eM), errorText(eT), caption(c)
	{};

	ErrorMsg(uint eM, std::wstring eT) : errorMsg(eM), errorText(eT), caption(L"Error")
	{};

	ErrorMsg(uint eM) : errorMsg(eM), errorText(L"An unfixable error has occurred."), caption(L"Error")
	{};

	void Print()
	{
		MessageBoxW(0, (errorText + L" Exit code: " + std::to_wstring(errorMsg) + L".").c_str(), caption.c_str(), 0);
	}
};
struct FinishMsg
{
	uint finishMsg;
	std::wstring text;
	std::wstring caption;

	FinishMsg(uint fM, std::wstring t, std::wstring c) : finishMsg(fM), text(t), caption(c)
	{};

	FinishMsg(uint fM, std::wstring t) : finishMsg(fM), text(t), caption(L"Exit")
	{};

	FinishMsg(uint fM) : finishMsg(fM), text(L"Appliation Exited Normally."), caption(L"Exit")
	{};

	void Print()
	{
		MessageBoxW(0, (text + L" Exit code: " + std::to_wstring(finishMsg) + L".").c_str() , caption.c_str(), 0);
	}
};

struct StateChange
{
	State* state;
	bool savePrevious;

	StateChange(State* pS, bool savePrev) : state(pS), savePrevious(savePrev)
	{ };
	StateChange(State* pS) : state(pS), savePrevious(false)
	{ };
};

struct VertexLayout
{
	DirectX::XMFLOAT3 _position;
	DirectX::XMFLOAT3 _normal;
	DirectX::XMFLOAT3 _tangent;
	DirectX::XMFLOAT3 _binormal;
	DirectX::XMFLOAT2 _texCoords;
};

struct SubMeshInfo
{
	uint32_t indexStart;
	uint32_t count;
};

struct BBT
{
	DirectX::BoundingOrientedBox root;
	DirectX::BoundingOrientedBox* children;
	unsigned int nrOfChildren;

	void Release()
	{
		if (children)
		{
			delete[] children;
		}
	}
};

// Macros
#define SAFE_SHUTDOWN(x) { if (x) { x->Shutdown(); delete (x); (x) = nullptr; } }
#define SAFE_DELETE(x) { if (x) { delete (x); (x) = nullptr; } }
#define SAFE_DELETE_ARRAY(x) { if (x) { delete[] (x); (x) = nullptr; } }

/// Keys
#define NROFKEYS 256

//#define VK_W 0x57
//#define VK_A 0x41
//#define VK_S 0x53
//#define VK_D 0x44
//#define VK_C 0x43
//#define VK_F 0x46
//#define VK_L 0x4C

#define VK_A 0x41
#define VK_B 0x42
#define VK_C 0x43
#define VK_D 0x44
#define VK_E 0x45
#define VK_F 0x46
#define VK_G 0x47
#define VK_H 0x48
#define VK_I 0x49
#define VK_J 0x4a
#define VK_K 0x4b
#define VK_L 0x4c
#define VK_M 0x4d
#define VK_N 0x4e
#define VK_O 0x4f
#define VK_P 0x50
#define VK_Q 0x51
#define VK_R 0x52
#define VK_S 0x53
#define VK_T 0x54
#define VK_U 0x55
#define VK_V 0x56
#define VK_W 0x57
#define VK_X 0x58
#define VK_Y 0x59
#define VK_Z 0x5a

#define NROFMOUSEKEYS 3

#endif