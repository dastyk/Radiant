#include "WindowHandler.h"
#include "System.h"


WindowHandler::WindowHandler()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_stateHandler = nullptr;
	_wndCaption = L"Radiant";
	_windowWidth = 800;
	_windowHeight = 640;
	_windowPosX = 0;
	_windowPosY = 0;
	_style = (WS_OVERLAPPED | WS_CAPTION);
}

WindowHandler::WindowHandler(uint windowWidth, uint windowHeight) : _windowWidth(windowWidth), _windowHeight(windowHeight)
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_stateHandler = nullptr;
	_wndCaption = L"Radiant";
	_windowPosX = 0;
	_windowPosY = 0;
	_style = (WS_OVERLAPPED | WS_CAPTION);
}


WindowHandler::~WindowHandler()
{
}

void WindowHandler::Init()
{
	_hInst = GetModuleHandle(NULL);

	Options* o = System::GetOptions();
	_windowPosX = o->GetWindowPosX();
	_windowPosY = o->GetWindowPosY();
	_screenWidth = _windowWidth = o->GetWindowWidth();
	_screenHeight = _windowHeight = o->GetWindowHeight();

	_InitWindow();

	try { _stateHandler = new StateHandler; }
	catch (std::exception& e)
	{
		e;
		throw ErrorMsg(2000001, L"Failed to create StateHandler");
	}
}

void WindowHandler::Shutdown()
{
	if (_stateHandler)
	{
		_stateHandler->Shutdown();
		delete _stateHandler;
		_stateHandler = nullptr;
	}
}

void WindowHandler::StartUp()
{
	_stateHandler->Init();

	MSG msg;
	while (true)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Do the frame processing.
		_stateHandler->Frame();
	}
}

const void WindowHandler::Move(uint xpos, uint ypos)
{
	_windowPosX = xpos;
	_windowPosY = ypos;
	return void();
}

const void WindowHandler::OnResize(uint width, uint height)
{
	_windowWidth = width;
	_windowHeight = height;

	return void();
}

const void WindowHandler::ToggleFullscreen()
{
	auto o = System::GetOptions();
	bool _fullscreen = o->GetFullscreen();
	if (!_fullscreen)
	{
		_screenWidth = _windowWidth = o->GetWindowWidth();
		_screenHeight = _windowHeight = o->GetWindowHeight();

		_windowPosX = (GetSystemMetrics(SM_CXSCREEN) - (int)_windowWidth) / 2;
		_windowPosY = (GetSystemMetrics(SM_CYSCREEN) - (int)_windowHeight) / 2;

		SetWindowLongPtr(_hWnd, GWL_STYLE, _style);
		RECT rc = { 0, 0, (LONG)_windowWidth, (LONG)_windowHeight };
		AdjustWindowRect(&rc, _style, FALSE);


		SetWindowPos(_hWnd,0, _windowPosX, _windowPosY, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);
		SetCursorPos(_windowPosX + _windowWidth / 2 - rc.left, _windowPosY + _windowHeight / 2 - rc.top);
		SetForegroundWindow(_hWnd);
		SetFocus(_hWnd);
		int r = ChangeDisplaySettings(0, 0) == DISP_CHANGE_SUCCESSFUL;
	}
	else
	{
		
		SetWindowLongPtr(_hWnd, GWL_STYLE,
			WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);

		_windowPosX = 0;
		_windowPosY = 0;
		_windowWidth = (uint)GetSystemMetrics(SM_CXSCREEN);
		_windowHeight = (uint)GetSystemMetrics(SM_CYSCREEN);
		_screenWidth = o->GetScreenResolutionWidth();
		_screenHeight = o->GetScreenResolutionHeight();
		SetWindowPos(_hWnd,0, _windowPosX, _windowPosY, _windowWidth, _windowHeight, SWP_SHOWWINDOW);
		SetForegroundWindow(_hWnd);
		SetFocus(_hWnd);


		DEVMODE dmWindowSettings;
		// If full Window set the Window to maximum size of the users desktop and 32bit.
		memset(&dmWindowSettings, 0, sizeof(dmWindowSettings));
		dmWindowSettings.dmSize = sizeof(dmWindowSettings);
		dmWindowSettings.dmPelsWidth = (unsigned long)_windowWidth;
		dmWindowSettings.dmPelsHeight = (unsigned long)_windowHeight;
		dmWindowSettings.dmBitsPerPel = 32;
		dmWindowSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		int r = ChangeDisplaySettings(&dmWindowSettings, 0) == DISP_CHANGE_SUCCESSFUL;

	}
	Input* i = System::GetInput();
	i->LockMouseToWindow(false);


	return void();
}

HWND WindowHandler::GetHWnd()
{
	return _hWnd;
}

const uint WindowHandler::GetWindowWidth() const
{
	return _windowWidth;
}

const uint WindowHandler::GetWindowHeight() const
{
	return _windowHeight;
}

const uint WindowHandler::GetScreenWidth() const
{
	return _screenWidth;
}

const uint WindowHandler::GetScreenHeight() const
{
	return _screenHeight;
}

const uint WindowHandler::GetWindowPosX() const
{
	return _windowPosX;
}

const uint WindowHandler::GetWindowPosY() const
{
	return _windowPosY;
}

const DWORD WindowHandler::GetStyle() const
{
	return _style;
}

const void WindowHandler::ChangeState(StateChange & c) const
{
	_stateHandler->ChangeState(c);
}



void WindowHandler::_InitWindow()
{
	// Setup the windows class
	WNDCLASSEX wc;

	System::GetInput();

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _hInst;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _wndCaption.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	//Place the window in the middle of the Window.
	_windowPosX = (GetSystemMetrics(SM_CXSCREEN) - (int)_windowWidth) / 2;
	_windowPosY = (GetSystemMetrics(SM_CYSCREEN) - (int)_windowHeight) / 2;

	RegisterClassEx(&wc);
	RECT rc = { 0, 0, (LONG)_windowWidth, (LONG)_windowHeight };

	AdjustWindowRect(&rc, _style, FALSE);
	// Create the window with the Window settings and get the handle to it.
	_hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		_wndCaption.c_str(),
		_wndCaption.c_str(),
		_style,
		_windowPosX,
		_windowPosY,
		rc.right - rc.left,
		rc.bottom - rc.top,
		NULL,
		NULL,
		_hInst,
		NULL);
	if (!_hWnd)
	{
		throw ErrorMsg(2000001, L"Failed to create Window");
	}


	auto o = System::GetOptions();
	bool _fullscreen = o->GetFullscreen();
	if (_fullscreen)
	{
		SetWindowLongPtr(_hWnd, GWL_STYLE,
			WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);

		_windowPosX = 0;
		_windowPosY = 0;
		_windowWidth = (uint)GetSystemMetrics(SM_CXSCREEN);
		_windowHeight = (uint)GetSystemMetrics(SM_CYSCREEN);
		_screenWidth = o->GetScreenResolutionWidth();
		_screenHeight = o->GetScreenResolutionHeight();
		SetWindowPos(_hWnd, 0, _windowPosX, _windowPosY, _windowWidth, _windowHeight, SWP_SHOWWINDOW);
		SetForegroundWindow(_hWnd);
		SetFocus(_hWnd);


		DEVMODE dmWindowSettings;
		// If full Window set the Window to maximum size of the users desktop and 32bit.
		memset(&dmWindowSettings, 0, sizeof(dmWindowSettings));
		dmWindowSettings.dmSize = sizeof(dmWindowSettings);
		dmWindowSettings.dmPelsWidth = (unsigned long)_windowWidth;
		dmWindowSettings.dmPelsHeight = (unsigned long)_windowHeight;
		dmWindowSettings.dmBitsPerPel = 32;
		dmWindowSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		int r = ChangeDisplaySettings(&dmWindowSettings, 0) == DISP_CHANGE_SUCCESSFUL;
	}

	// Bring the window up on the Window and set it as main focus.
	ShowWindow(_hWnd, SW_SHOW);
	SetForegroundWindow(_hWnd);
	SetFocus(_hWnd);

	// Set the cursor to the middle of the client window
	SetCursorPos(_windowPosX + _windowWidth / 2, _windowPosY + _windowHeight / 2);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		break;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		//return DefWindowProc(hwnd, umessage, wparam, lparam);
		return System::GetInput()->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
	return 0;
}
