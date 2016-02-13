#include "WindowHandler.h"
#include "System.h"


WindowHandler::WindowHandler()
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
	_running = true;
	MSG msg;
	while (_running)
	{
		
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		System::GetInput()->Frame();

		// Do the frame processing.
		_stateHandler->Frame();
	}
}

const void WindowHandler::Exit()
{
	_running = false;
}

const void WindowHandler::OnResize()
{
	auto o = System::GetOptions();
	bool fullscreen = o->GetFullscreen();
	LONG windowWidth = (LONG)o->GetScreenResolutionWidth();
	LONG windowHeight = (LONG)o->GetScreenResolutionHeight();

	if (!fullscreen)
	{
		
		_windowPosX = (GetSystemMetrics(SM_CXSCREEN) - (int)windowWidth) / 2;
		_windowPosY = (GetSystemMetrics(SM_CYSCREEN) - (int)windowHeight) / 2;

		SetWindowLongPtr(_hWnd, GWL_STYLE, _style);
		RECT rc = { 0, 0, windowWidth, windowHeight };
		AdjustWindowRect(&rc, _style, FALSE);


		SetWindowPos(_hWnd, 0, _windowPosX, _windowPosY, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);
		SetCursorPos(_windowPosX + windowWidth / 2 - rc.left, _windowPosY + windowHeight / 2 - rc.top);
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
		windowWidth = (uint)GetSystemMetrics(SM_CXSCREEN);
		windowHeight = (uint)GetSystemMetrics(SM_CYSCREEN);
		SetWindowPos(_hWnd, 0, _windowPosX, _windowPosY, windowWidth, windowHeight, SWP_SHOWWINDOW);
		SetForegroundWindow(_hWnd);
		SetFocus(_hWnd);


		DEVMODE dmWindowSettings;
		// If full Window set the Window to maximum size of the users desktop and 32bit.
		memset(&dmWindowSettings, 0, sizeof(dmWindowSettings));
		dmWindowSettings.dmSize = sizeof(dmWindowSettings);
		dmWindowSettings.dmPelsWidth = (unsigned long)windowWidth;
		dmWindowSettings.dmPelsHeight = (unsigned long)windowHeight;
		dmWindowSettings.dmBitsPerPel = 32;
		dmWindowSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		int r = ChangeDisplaySettings(&dmWindowSettings, 0) == DISP_CHANGE_SUCCESSFUL;
	}
	return void();
}

const void WindowHandler::OnMove(uint xpos, uint ypos)
{
	_windowPosX = xpos;
	_windowPosY = ypos;
}


HWND WindowHandler::GetHWnd()
{
	return _hWnd;
}

const int WindowHandler::GetWindowPosX() const
{
	return _windowPosX;
}

const int WindowHandler::GetWindowPosY() const
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

	auto o = System::GetOptions();
	bool fullscreen = o->GetFullscreen();
	LONG windowWidth = (LONG)o->GetScreenResolutionWidth();
	LONG windowHeight = (LONG)o->GetScreenResolutionHeight();


	// Register the window class.
	//Place the window in the middle of the Window.
	_windowPosX = (GetSystemMetrics(SM_CXSCREEN) - (int)windowWidth) / 2;
	_windowPosY = (GetSystemMetrics(SM_CYSCREEN) - (int)windowHeight) / 2;

	RegisterClassEx(&wc);
	RECT rc = { 0, 0, (LONG)windowWidth, (LONG)windowHeight };

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

	if (fullscreen)
	{
		SetWindowLongPtr(_hWnd, GWL_STYLE,
			WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);

		_windowPosX = 0;
		_windowPosY = 0;
		windowWidth = (LONG)GetSystemMetrics(SM_CXSCREEN);
		windowHeight = (LONG)GetSystemMetrics(SM_CYSCREEN);
		SetWindowPos(_hWnd, 0, _windowPosX, _windowPosY, windowWidth, windowHeight, SWP_SHOWWINDOW);
		SetForegroundWindow(_hWnd);
		SetFocus(_hWnd);


		DEVMODE dmWindowSettings;
		// If full Window set the Window to maximum size of the users desktop and 32bit.
		memset(&dmWindowSettings, 0, sizeof(dmWindowSettings));
		dmWindowSettings.dmSize = sizeof(dmWindowSettings);
		dmWindowSettings.dmPelsWidth = (unsigned long)windowWidth;
		dmWindowSettings.dmPelsHeight = (unsigned long)windowHeight;
		dmWindowSettings.dmBitsPerPel = 32;
		dmWindowSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		int r = ChangeDisplaySettings(&dmWindowSettings, 0) == DISP_CHANGE_SUCCESSFUL;

		System::GetInput()->LockMouseToWindow(true);
	}

	// Bring the window up on the Window and set it as main focus.
	ShowWindow(_hWnd, SW_SHOW);
	SetForegroundWindow(_hWnd);
	SetFocus(_hWnd);

	// Set the cursor to the middle of the client window
	SetCursorPos(_windowPosX + windowWidth / 2, _windowPosY + windowHeight / 2);
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
