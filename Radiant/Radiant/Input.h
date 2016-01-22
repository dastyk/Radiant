#ifndef _INPUT_H_
#define _INPUT_H_

#pragma once

//////////////
// Includes //
//////////////

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
	const void ShutDown();

	const void KeyDown(uint keyCode);
	const void KeyUp(uint keyCode);

	const bool IsKeyDown(uint keyCode)const;

	const bool GetKeyStateAndReset(uint keyCode);

	const void OnMouseMove(int x, int y);
	const void MouseDown(uint keyCode);
	const void MouseUp(uint keyCode);

	const bool IsMouseKeyDown(uint keyCode)const;
	const bool GetMouseKeyStateAndReset(uint keyCode);

	const void GetMousePos(int &x, int &y)const;

private:
	bool _keys[NROFKEYS];
	bool _mouseKeys[NROFMOUSEKEYS];

	int _mousePosX, _mousePosY, _lastMousePosX, _lastMousePosY;
};

#endif