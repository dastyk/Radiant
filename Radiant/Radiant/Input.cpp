#include "Input.h"
#include "System.h"


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

	_mousePosX = 0;
	_mousePosY = 0;
	_lastMousePosX = 0;
	_lastMousePosY = 0;

	_mouseLockedToScreen = false;
	_mouseLockedToCenter = false;

	return void();
}

const void Input::Shutdown()
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

const void Input::OnMouseMove(unsigned int x, unsigned int y)
{
	WindowHandler* h = System::GetWindowHandler();

	if (_mouseLockedToCenter)
	{
		
		uint wW = h->GetWindowWidth();
		uint wH = h->GetWindowHeight();
		uint wX = h->GetWindowPosX();
		uint wY = h->GetWindowPosY();

		_lastMousePosX = x;
		_lastMousePosY = y;
		_mousePosX = wW / 2;
		_mousePosY = wH / 2;



		RECT rc = { 0,0,0,0 };
		AdjustWindowRect(&rc, h->GetStyle(), FALSE);


		SetCursorPos(wX + _mousePosX - rc.left, wY + _mousePosY - rc.top);

		
	}
	else if (_mouseLockedToScreen)
	{
		///WindowHandler* h = System::GetWindowHandler();

		uint wW = h->GetWindowWidth();
		uint wH = h->GetWindowHeight();

		_lastMousePosX = _mousePosX;
		_lastMousePosY = _mousePosY;

		_mousePosX = (x >= wW) ? wW : x;
		_mousePosY = (y >= wH) ? wH : y;

		_mousePosX = (x < 0) ? 0 : x;
		_mousePosY = (y < 0) ? 0 : y;

	
	}
	else
	{
		_lastMousePosX = _mousePosX;
		_lastMousePosY = _mousePosY;
		_mousePosX = x;
		_mousePosY = y;
	}

	
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

const void Input::GetMousePos(int& rX, int& rY) const
{
	rX = _mousePosX;
	rY = _mousePosY;
	return void();
}

const void Input::GetMouseDiff(int& rX, int& rY) const
{

	rX = _lastMousePosX -  _mousePosX;
	rY = _lastMousePosY -  _mousePosY;
	return void();
}

const void Input::ToggleLockMouseToCenter()
{
	if (_mouseLockedToCenter)
	{
	
		_mouseLockedToCenter = false;
	}
	else
	{
		_mouseLockedToCenter = true;
	}
	return void();
}

const void Input::ToggleLockMouseToWindow()
{
	if (_mouseLockedToScreen)
	{
		ClipCursor(nullptr);
		_mouseLockedToScreen = false;
	}
	else
	{
		WindowHandler* h = System::GetWindowHandler();
		RECT clipping;
		clipping.left = 0;
		clipping.right = h->GetWindowWidth();
		clipping.top = 0;
		clipping.bottom = h->GetWindowHeight();
		if (h->IsFullscreen())
		{
			ClipCursor(&clipping);
		}
		else
		{
			

			RECT rc = clipping;
			AdjustWindowRect(&rc, h->GetStyle(), FALSE);

			RECT rcClip;           // new area for ClipCursor

			GetWindowRect(h->GetHWnd(), &rcClip);
			rcClip.right -= rc.right - clipping.right;
			rcClip.bottom -= rc.bottom - clipping.bottom;
			rcClip.left -= rc.left - clipping.left;
			rcClip.top -= rc.top - clipping.top;
			// Confine the cursor to the application's window. 

			ClipCursor(&rcClip);
		}
		
		_mouseLockedToScreen = true;
	}
	return void();
}

const void Input::HideCursor(bool show) const
{
	ShowCursor(!show);
	return void();
}

LRESULT Input::MessageHandler(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	WindowHandler* h = System::GetWindowHandler();
	switch (umsg)
	{
	case WM_MOVE:
	{
		//The windows moved, tell the window handler.
		RECT rc = { 0,0,0,0 };
		AdjustWindowRect(&rc, h->GetStyle(), FALSE);

		h->Move(LOWORD(lParam) + rc.left, HIWORD(lParam) + rc.top);
		break;
	}
	case WM_SIZE:
		h->OnResize(LOWORD(lParam), HIWORD(lParam));
		ToggleLockMouseToWindow();
		ToggleLockMouseToWindow();
		break;
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