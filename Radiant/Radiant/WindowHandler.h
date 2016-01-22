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
	~WindowHandler();

	//Initialize Functions
	void Init();
	void ShutDown();

	//Game Loop is placed inside the StartUp
	void StartUp();

	//Loop Functions
	void Frame();

	// Application functions	
	//void Resize(uint x, uint y, uint width, uint height);

	// State functions
	//void ChangeState(uint8 state);
	//void CreateState(uint8 state);
	//void ShutDownState(uint8 state);
//	ApplicationState* GetState(uint8 state);

	// Get/set
	HWND GetHWnd();


	//Message Handling
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);

private:
	//Variables for the Window
	HINSTANCE _hInst;
	HWND _hWnd;

	std::wstring _wndCaption;

	StateHandler* _stateHandler;

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