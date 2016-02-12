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

#define ExitApplication System::GetWindowHandler()->Exit();

class WindowHandler
{
public:
	WindowHandler();
	~WindowHandler();

	//Initialize Functions
	void Init();
	void Shutdown();

	//Game Loop is placed inside the StartUp
	void StartUp();
	const void Exit();

	// Application functions	
	const void OnResize();
	const void OnMove(uint xpos, uint ypos);

	// State functions
	//void ChangeState(uint8 state);
	//void CreateState(uint8 state);
	//void ShutDownState(uint8 state);
//	ApplicationState* GetState(uint8 state);

	// Get/set
	HWND GetHWnd();

	const int GetWindowPosX()const;
	const int GetWindowPosY()const;
	const DWORD GetStyle()const;

	const void ChangeState(StateChange& c)const;
private:
	//Variables for the Window
	HINSTANCE _hInst;
	HWND _hWnd;
	DWORD _style;
	std::wstring _wndCaption;
	StateHandler* _stateHandler;

	int _windowPosX, _windowPosY;
	bool _running = false;
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