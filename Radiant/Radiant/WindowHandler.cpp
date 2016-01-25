#include "WindowHandler.h"
#include "System.h"


WindowHandler::WindowHandler()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_stateHandler = nullptr;
	_wndCaption = L"Radiant";
	_windowWidth = 800;
	_windowHeight = 600;
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

	_InitWindow();

	try { _stateHandler = new StateHandler; }
	catch (std::exception& e)
	{
		throw ErrorMsg(2000001, L"Failed to create StateHandler");
	}

	_stateHandler->Init();
}

void WindowHandler::ShutDown()
{
	if (_stateHandler)
	{
		_stateHandler->ShutDown();
		delete _stateHandler;
		_stateHandler = nullptr;
	}
}

void WindowHandler::StartUp()
{
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



void WindowHandler::_InitWindow()
{
	// Setup the windows class
	WNDCLASSEX wc;

	System::GetInstance()->GetInput();

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
	RegisterClassEx(&wc);

	//// Setup the Window settings depending on whether it is running in full Window or in windowed mode.
	//if (mFullWindow)
	//{

	//	mClientWidth = mWindowWidth;
	//	mClientHeight = mWindowHeight;

	//	DEVMODE dmWindowSettings;
	//	// If full Window set the Window to maximum size of the users desktop and 32bit.
	//	memset(&dmWindowSettings, 0, sizeof(dmWindowSettings));
	//	dmWindowSettings.dmSize = sizeof(dmWindowSettings);
	//	dmWindowSettings.dmPelsWidth = (unsigned long)mClientWidth;
	//	dmWindowSettings.dmPelsHeight = (unsigned long)mClientHeight;
	//	dmWindowSettings.dmBitsPerPel = 32;
	//	dmWindowSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	//	// Change the display settings to full Window.
	//	ChangeDisplaySettings(&dmWindowSettings, CDS_FULLWindow);

	//	// Set the position of the window to the top left corner.
	//	posX = posY = 0;
	//}
	//else
	//{
	// Place the window in the middle of the Window.
	_windowPosX = (GetSystemMetrics(SM_CXSCREEN) - (int)_windowWidth) / 2;
	_windowPosY = (GetSystemMetrics(SM_CYSCREEN) - (int)_windowHeight) / 2;

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
		return System::GetInstance()->GetInput()->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
	return 0;
}
