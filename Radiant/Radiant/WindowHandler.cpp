#include "WindowHandler.h"
#include "System.h"


WindowHandler::WindowHandler()
{
	_hInst = nullptr;
	_hWnd = nullptr;

	_running = false;

	_wndCaption = L"Radiant";

}


WindowHandler::~WindowHandler()
{
}

void WindowHandler::Init()
{
	_hInst = GetModuleHandle(NULL);

	_InitWindow();
}

void WindowHandler::ShutDown()
{
}

void WindowHandler::StartUp()
{
	MSG msg;
	_running = true;
	while (_running)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Do the frame processing.
		Frame();
	}
}

void WindowHandler::Frame()
{
	_HandleInput();
	_Update();
	_Render();
}

void WindowHandler::ExitApp()
{
	_running = false;
}

HWND WindowHandler::GetHWnd()
{
	return _hWnd;
}

LRESULT WindowHandler::MessageHandler(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{

		//check if a key has been pressed on the keyboard
	case WM_KEYDOWN:
	{
		//If a key is pressed send it to the input object so it can record that state
		break;
	}
	//check if a key has been released
	case WM_KEYUP:
	{
		//If a key is released send it to the input object so it can record that state
		break;
	}
	// Check if a key on the mouse has been pressed.
	case WM_LBUTTONDOWN:
	{
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		return 0;
	}
	// Check if a key on the mouse has been released.
	case WM_LBUTTONUP:
	{
		return 0;
	}
	case WM_MBUTTONUP:
	{
		return 0;
	}
	case WM_RBUTTONUP:
	{
		return 0;
	}
	// Check if mouse has been moved.
	case WM_MOUSEMOVE:
	{
		return 0;
	}

	//Send every other message to the default message handler
	default:
	{
		return DefWindowProc(hwnd, umsg, wParam, lParam);
	}

	}
	return 0;
}

void WindowHandler::_InitWindow()
{
	// Setup the windows class
	WNDCLASSEX wc;
	int posX, posY;

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

	//// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	//if (mFullscreen)
	//{

	//	mClientWidth = mScreenWidth;
	//	mClientHeight = mScreenHeight;

	//	DEVMODE dmScreenSettings;
	//	// If full screen set the screen to maximum size of the users desktop and 32bit.
	//	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
	//	dmScreenSettings.dmSize = sizeof(dmScreenSettings);
	//	dmScreenSettings.dmPelsWidth = (unsigned long)mClientWidth;
	//	dmScreenSettings.dmPelsHeight = (unsigned long)mClientHeight;
	//	dmScreenSettings.dmBitsPerPel = 32;
	//	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	//	// Change the display settings to full screen.
	//	ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

	//	// Set the position of the window to the top left corner.
	//	posX = posY = 0;
	//}
	//else
	//{
	// Place the window in the middle of the screen.
	posX = (GetSystemMetrics(SM_CXSCREEN) - 800) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - 600) / 2;

	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, (WS_OVERLAPPED | WS_CAPTION), FALSE);


	// Create the window with the screen settings and get the handle to it.
	_hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		_wndCaption.c_str(),
		_wndCaption.c_str(),
		(WS_OVERLAPPED | WS_CAPTION),
		posX,
		posY,
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


	// Bring the window up on the screen and set it as main focus.
	ShowWindow(_hWnd, SW_SHOW);
	SetForegroundWindow(_hWnd);
	SetFocus(_hWnd);

	// Set the cursor to the middle of the client window
	SetCursorPos(posX + 800 / 2, posY + 600 / 2);

}

void WindowHandler::_HandleInput()
{
	// Call the HandleInput in current state
	
}

void WindowHandler::_Update()
{
	// Call the Update in current state

}

void WindowHandler::_Render()
{
	// Call the Render in current state

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
		return System::GetInstance()->_windowHandler->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
	return 0;
}
