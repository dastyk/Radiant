#ifndef _INPUT_H_
#define _INPUT_H_

#pragma once

//////////////
// Includes //
//////////////
#include <windowsx.h>

////////////////////
// Local Includes //
////////////////////
#include "General.h"

class Input
{
public:
	Input();
	~Input();

	const void Init();
	const void Shutdown();

	const void KeyDown(uint keyCode);
	const void KeyUp(uint keyCode);

	const bool IsKeyDown(uint keyCode)const;

	const bool GetKeyStateAndReset(uint keyCode);

	const void OnMouseMove(unsigned int x, unsigned int y);
	const void MouseDown(uint keyCode);
	const void MouseUp(uint keyCode);

	const bool IsMouseKeyDown(uint keyCode)const;
	const bool GetMouseKeyStateAndReset(uint keyCode);

	const void GetMousePos(int& rX, int& rY)const;
	const void GetMouseDiff(int& rX, int& rY)const;

	const void LockMouseToCenter(bool lock);
	const void LockMouseToWindow(bool lock);
	const void HideCursor(bool show)const;

	//Message Handling
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
private:
	bool _keys[NROFKEYS];
	bool _mouseKeys[NROFMOUSEKEYS];

	int _mousePosX, _mousePosY, _lastMousePosX, _lastMousePosY;

	bool _mouseLockedToScreen;
	bool _mouseLockedToCenter;

};

#endif