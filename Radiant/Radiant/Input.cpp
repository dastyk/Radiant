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
		_keyPressed[i] = false;
	}
	for (uint i = 0; i < NROFMOUSEKEYS; i++)
	{
		_mouseKeys[i] = false;
		_mouseKeyPressed[i] = false;
	}
	_mousePosX = 0;
	_mousePosY = 0;
	_lastMousePosX = 0;
	_lastMousePosY = 0;

	_mouseLockedToScreen = false;
	_mouseLockedToCenter = false;


	//_device[0].usUsagePage = 0x01;
	//_device[0].usUsage = 0x02;
	//_device[0].dwFlags = RIDEV_NOLEGACY;   // adds HID mouse and also ignores legacy mouse messages
	//_device[0].hwndTarget = 0;

	//_device[1].usUsagePage = 0x01;
	//_device[1].usUsage = 0x06;
	//_device[1].dwFlags = RIDEV_NOLEGACY;   // adds HID keyboard and also ignores legacy keyboard messages
	//_device[1].hwndTarget = 0;

	//if (RegisterRawInputDevices(&_device[1], 1, sizeof(RAWINPUTDEVICE)) == FALSE) 
	//{
	//	throw ErrorMsg(4000003, L"Could not register raw input device");
	//}

	return void();
}

const void Input::Shutdown()
{

	return void();
}

const void Input::Frame()//HRAWINPUT lParam)
{
	WindowHandler* h = System::GetWindowHandler();
	auto o = System::GetOptions();
	POINT p;

	uint wW = o->GetScreenResolutionWidth();
	uint wH = o->GetScreenResolutionHeight();

	GetCursorPos(&p);

	ScreenToClient(h->GetHWnd(), &p);


	_mousePosX = (int)p.x;
	_mousePosY = (int)p.y;

	_xDiff = _lastMousePosX - _mousePosX;
	_yDiff = _lastMousePosY - _mousePosY;



	if (o->GetFullscreen())
	{
		wW = GetSystemMetrics(SM_CXSCREEN);
		wH = GetSystemMetrics(SM_CYSCREEN);
	}
	if (_mouseLockedToCenter)
	{
		_lastMousePosX = _mousePosX = wW / 2;
		_lastMousePosY = _mousePosY = wH / 2;

		uint wX = h->GetWindowPosX();
		uint wY = h->GetWindowPosY();
		p.x = _mousePosX;
		p.y = _mousePosY;
		ClientToScreen(h->GetHWnd(), &p);

		//RECT rc = { 0,0,0,0 };
		//AdjustWindowRect(&rc, h->GetStyle(), FALSE);

		SetCursorPos(p.x, p.y);//SetCursorPos(wX + _mousePosX - rc.left, wY + _mousePosY - rc.top);
	}
	else
	{
		_lastMousePosX = _mousePosX;
		_lastMousePosY = _mousePosY;
	}
	

	//for (uint i = 0; i < NROFKEYS; i++)
	//{
	//	if (HIBYTE(GetKeyState(i)))
	//		KeyDown(i);
	//	else
	//		KeyUp(i);
	//}

	_scrollDelta = 0;

	return void();
}

const void Input::KeyDown(uint keyCode)
{
	if (keyCode >= NROFKEYS)
		throw ErrorMsg(4000001, L"Key out of bounds" + std::to_wstring(keyCode));
	_keys[keyCode] = true;
	return void();
}

const void Input::KeyUp(uint keyCode)
{
	if (keyCode >= NROFKEYS)
		throw ErrorMsg(4000001, L"Key out of bounds" + std::to_wstring(keyCode));
	_keys[keyCode] = false;
	_keyPressed[keyCode] = false;
	return void();
}

const bool Input::IsKeyDown(uint keyCode) const
{
	if (keyCode >= NROFKEYS)
		throw ErrorMsg(4000001, L"Key out of bounds" + std::to_wstring(keyCode));
	return _keys[keyCode];
}

const bool Input::IsKeyPushed(uint keyCode)
{
	if (keyCode >= NROFKEYS)
		throw ErrorMsg(4000001, L"Key out of bounds" + std::to_wstring(keyCode));
	bool out = false;
	if (!_keyPressed[keyCode] && _keys[keyCode])
	{
		out = true;
		_keyPressed[keyCode] = true;
	}
	return out;
}

const void Input::OnMouseMove(unsigned int x, unsigned int y)
{
	_mousePosX = x;
	_mousePosY = y;
	return void();
}

const void Input::MouseDown(uint keyCode)
{
	if (keyCode >= NROFMOUSEKEYS)
		throw ErrorMsg(4000002, L"Mouse Key out of bounds" + std::to_wstring(keyCode));
	_mouseKeys[keyCode] = true;
	return void();
}

const void Input::MouseUp(uint keyCode)
{
	if (keyCode >= NROFMOUSEKEYS)
		throw ErrorMsg(4000002, L"Mouse Key out of bounds" + std::to_wstring(keyCode));
	_mouseKeys[keyCode] = false;
	_mouseKeyPressed[keyCode] = false;
	return void();
}

const bool Input::IsScrollDown(int & delta)
{
	delta = _scrollDelta;
	return delta < 0 ? true : false;
}

const bool Input::IsScrollUp(int & delta)
{
	delta = _scrollDelta;
	return delta > 0 ? true : false;
}

void Input::OnMouseScroll(int delta)
{
	_scrollDelta = delta;
}

const bool Input::IsMouseKeyDown(uint keyCode) const
{
	if (keyCode >= NROFMOUSEKEYS)
		throw ErrorMsg(4000002, L"Mouse Key out of bounds" + std::to_wstring(keyCode));
	return _mouseKeys[keyCode];
}

const bool Input::IsMouseKeyPushed(uint keyCode)
{
	if (keyCode >= NROFMOUSEKEYS)
		throw ErrorMsg(4000002, L"Mouse Key out of bounds" + std::to_wstring(keyCode));
	bool out = false;
	if (!_mouseKeyPressed[keyCode] && _mouseKeys[keyCode])
	{
		out = true;
		_mouseKeyPressed[keyCode] = true;
	}
	return out;
}

const void Input::GetMousePos(int& rX, int& rY) const
{
	auto o = System::GetOptions();
	if (o->GetFullscreen())
	{
		float sw = (float)GetSystemMetrics(SM_CXSCREEN);
		float sh = (float)GetSystemMetrics(SM_CYSCREEN);
		float ww = (float)o->GetScreenResolutionWidth();
		float wh = (float)o->GetScreenResolutionHeight();
		float pw = ww / sw;
		float ph = wh / sh;
		rX = static_cast<int>(_mousePosX*pw);
		rY = static_cast<int>(_mousePosY*ph);
		
	
	}
	else
	{
		rX = _mousePosX;
		rY = _mousePosY;
	}
//	OutputDebugStringW((to_wstring(_mousePosX) + L" " + to_wstring(_mousePosY) + L" \n").c_str());
	return void();
}

const void Input::GetMouseDiff(int& rX, int& rY) const
{
	rX = -_xDiff;
	rY = -_yDiff;
	return void();
}

const void Input::LockMouseToCenter(bool lock)
{
	auto h = System::GetWindowHandler();
	auto o = System::GetOptions();
	uint wW = o->GetScreenResolutionWidth();
	uint wH = o->GetScreenResolutionHeight();

	if (o->GetFullscreen())
	{
		wW = GetSystemMetrics(SM_CXSCREEN);
		wH = GetSystemMetrics(SM_CYSCREEN);
	}

	uint wX = h->GetWindowPosX();
	uint wY = h->GetWindowPosY();

	_lastMousePosX = wW / 2;
	_lastMousePosY = wH / 2;
	_mousePosX = wW / 2;
	_mousePosY = wH / 2;

	RECT rc = { 0,0,0,0 };
	AdjustWindowRect(&rc, h->GetStyle(), FALSE);

	if(lock)
		SetCursorPos(wX + _mousePosX - rc.left, wY + _mousePosY - rc.top);
	_mouseLockedToCenter = lock;
	return void();
}

const void Input::LockMouseToWindow(bool lock)
{
	if (lock)
	{
		WindowHandler* h = System::GetWindowHandler();
		auto o = System::GetOptions();
		RECT clipping;
		clipping.left = 0;
		clipping.right = o->GetScreenResolutionWidth();
		clipping.top = 0;
		clipping.bottom = o->GetScreenResolutionHeight();
		if (o->GetFullscreen())
		{
			clipping.right = GetSystemMetrics(SM_CXSCREEN);
			clipping.bottom = GetSystemMetrics(SM_CYSCREEN);
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
	else
	{
		ClipCursor(nullptr);
		_mouseLockedToScreen = false;
	}
	return void();
}

const void Input::HideCursor(bool show) const
{
	if (show)
	{
		while (ShowCursor(!show) >= 0);
	}
	else
		while (ShowCursor(!show) < 0);
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

		h->OnMove(LOWORD(lParam) + rc.left, HIWORD(lParam) + rc.top);
		break;
	}
	case WM_SIZE:
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
		MouseDown((uint)VK_LBUTTON);
		break;
	}
	case WM_MBUTTONDOWN:
	{
		MouseDown((uint)VK_MBUTTON);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		MouseDown((uint)VK_RBUTTON);
		break;
	}
	// Check if a key on the mouse has been released.
	case WM_LBUTTONUP:
	{
		MouseUp((uint)VK_LBUTTON);
		break;
	}
	case WM_MBUTTONUP:
	{
		MouseUp((uint)VK_MBUTTON);
		break;
	}
	case WM_RBUTTONUP:
	{
		MouseUp((uint)VK_RBUTTON);
		break;
	}
	// Check if mouse has been moved.
	case WM_MOUSEMOVE:
	{
		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	}
	case WM_MOUSEWHEEL:
	{
		OnMouseScroll(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	}
	case WM_INPUT:
		//Frame((HRAWINPUT)lParam);
		break;
	//Send every other message to the default message handler
	default:
	{
		return DefWindowProc(hwnd, umsg, wParam, lParam);
	}

	}
	return 0;
}