#pragma once
#ifndef _WINDOWHANDLER_H_
#define _WINDOWHANDLER_H_

#pragma once
//////////////
// Includes //
//////////////
#include <Windows.h>
#include <windowsx.h>
#include <sstream>
#include <string>


////////////////////
// Local Includes //
////////////////////
#include "General.h"
#include "StateHandler.h"

class WindowHandler
{
public:
	WindowHandler();
	WindowHandler(uint WindowWidth, uint WindowHeight);
	~WindowHandler();

	//Initialize Functions
	void Init();
	void Shutdown();

	//Game Loop is placed inside the StartUp
	void StartUp();

	const void Move(uint xpos, uint ypos);
	// Application functions	
	const void OnResize(uint width, uint height);
	const void ToggleFullscreen();

	// State functions
	//void ChangeState(uint8 state);
	//void CreateState(uint8 state);
	//void ShutDownState(uint8 state);
//	ApplicationState* GetState(uint8 state);

	// Get/set
	HWND GetHWnd();

	const uint GetWindowWidth()const;
	const uint GetWindowHeight()const;
	const uint GetScreenWidth()const;
	const uint GetScreenHeight()const;
	const uint GetWindowPosX()const;
	const uint GetWindowPosY()const;
	const DWORD GetStyle()const;
	const bool IsFullscreen()const;

private:
	//Variables for the Window
	HINSTANCE _hInst;
	HWND _hWnd;
	DWORD _style;
	std::wstring _wndCaption;
	bool _fullscreen;
	StateHandler* _stateHandler;

	uint _windowWidth, _windowHeight;
	uint _screenWidth, _screenHeight;
	uint _windowPosX, _windowPosY;
	// States
	//ApplicationState* mCurrState;
	//ApplicationState* mStates[STATES_NROFSTATES];

private:
	//Initialize Functions
	void _InitWindow();


};

//WndProc and ApplicationHandle is used to make sure we can handle windows messages
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#endif