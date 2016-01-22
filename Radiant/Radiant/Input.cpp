#include "Input.h"



Input::Input()
{
}


Input::~Input()
{
}

const void Input::Init()
{
	for (uint i = 0; i < NROFKEYS; i++)
	{
		_keys[i] = false;
	}


	return void();
}

const void Input::ShutDown()
{

	return void();
}

const void Input::KeyDown(uint keyCode)
{
	if (keyCode >= NROFKEYS || keyCode < 0)
		throw ErrorMsg(4000001, L"Key out of bounds" + std::to_wstring(keyCode));
	_keys[keyCode] = true;
	return void();
}

const void Input::KeyUp(uint keyCode)
{
	if (keyCode >= NROFKEYS || keyCode < 0)
		throw ErrorMsg(4000001, L"Key out of bounds" + std::to_wstring(keyCode));
	_keys[keyCode] = false;
	return void();
}

const bool Input::IsKeyDown(uint keyCode) const
{
	if (keyCode >= NROFKEYS || keyCode < 0)
		throw ErrorMsg(4000001, L"Key out of bounds" + std::to_wstring(keyCode));
	return _keys[keyCode];
}

const bool Input::GetKeyStateAndReset(uint keyCode)
{
	if (keyCode >= NROFKEYS || keyCode < 0)
		throw ErrorMsg(4000001, L"Key out of bounds" + std::to_wstring(keyCode));
	bool out = _keys[keyCode];
	_keys[keyCode] = false;
	return out;
}

const void Input::OnMouseMove(int x, int y)
{
	_lastMousePosX = _mousePosX;
	_lastMousePosY = _mousePosY;
	_mousePosX = x;
	_mousePosY = y;
	return void();
}

const void Input::MouseDown(uint keyCode)
{
	if (keyCode >= NROFMOUSEKEYS || keyCode < 0)
		throw ErrorMsg(4000002, L"Mouse Key out of bounds" + std::to_wstring(keyCode));
	_mouseKeys[keyCode] = true;
	return void();
}

const void Input::MouseUp(uint keyCode)
{
	if (keyCode >= NROFMOUSEKEYS || keyCode < 0)
		throw ErrorMsg(4000002, L"Mouse Key out of bounds" + std::to_wstring(keyCode));
	_mouseKeys[keyCode] = true;
	return void();
}

const bool Input::IsMouseKeyDown(uint keyCode) const
{
	if (keyCode >= NROFMOUSEKEYS || keyCode < 0)
		throw ErrorMsg(4000002, L"Mouse Key out of bounds" + std::to_wstring(keyCode));
	return _mouseKeys[keyCode];
}

const bool Input::GetMouseKeyStateAndReset(uint keyCode)
{
	if (keyCode >= NROFMOUSEKEYS || keyCode < 0)
		throw ErrorMsg(4000002, L"Mouse Key out of bounds" + std::to_wstring(keyCode));
	bool out = _mouseKeys[keyCode];
	_mouseKeys[keyCode] = false;
	return out;
}

const void Input::GetMousePos(int & x, int & y) const
{
	x = _mousePosX;
	y = _mousePosY;
	return void();
}

LRESULT Input::MessageHandler(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{

		//check if a key has been pressed on the keyboard
	case WM_KEYDOWN:
	{
		//If a key is pressed send it to the input object so it can record that state
		KeyDown((uint)wParam);
		break;
	}
	//check if a key has been released
	case WM_KEYUP:
	{
		//If a key is released send it to the input object so it can record that state
		KeyUp((uint)wParam);
		break;
	}
	// Check if a key on the mouse has been pressed.
	case WM_LBUTTONDOWN:
	{
		MouseDown((uint)LMOUSE);
		break;
	}
	case WM_MBUTTONDOWN:
	{
		MouseDown((uint)MMOUSE);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		MouseDown((uint)RMOUSE);
		break;
	}
	// Check if a key on the mouse has been released.
	case WM_LBUTTONUP:
	{
		MouseUp((uint)LMOUSE);
		break;
	}
	case WM_MBUTTONUP:
	{
		MouseUp((uint)MMOUSE);
		break;
	}
	case WM_RBUTTONUP:
	{
		MouseUp((uint)RMOUSE);
		break;
	}
	// Check if mouse has been moved.
	case WM_MOUSEMOVE:
	{
		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	}

	//Send every other message to the default message handler
	default:
	{
		return DefWindowProc(hwnd, umsg, wParam, lParam);
	}

	}
	return 0;
}