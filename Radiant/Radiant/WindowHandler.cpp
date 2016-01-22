#include "WindowHandler.h"
#include "System.h"


WindowHandler::WindowHandler()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_stateHandler = nullptr;
	_wndCaption = L"Radiant";

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

LRESULT WindowHandler::MessageHandler(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	Input* i = System::GetInstance()->_inputInst;
	switch (umsg)
	{

		//check if a key has been pressed on the keyboard
	case WM_KEYDOWN:
	{
		//If a key is pressed send it to the input object so it can record that state
		i->KeyDown((uint)wParam);
		break;
	}
	//check if a key has been released
	case WM_KEYUP:
	{
		//If a key is released send it to the input object so it can record that state
		i->KeyUp((uint)wParam);
		break;
	}
	// Check if a key on the mouse has been pressed.
	case WM_LBUTTONDOWN:
	{
		i->MouseDown((uint)LMOUSE);
		break;
	}
	case WM_MBUTTONDOWN:
	{
		i->MouseDown((uint)MMOUSE);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		i->MouseDown((uint)RMOUSE);
		break;
	}
	// Check if a key on the mouse has been released.
	case WM_LBUTTONUP:
	{
		i->MouseUp((uint)LMOUSE);
		break;
	}
	case WM_MBUTTONUP:
	{
		i->MouseUp((uint)MMOUSE);
		break;
	}
	case WM_RBUTTONUP:
	{
		i->MouseUp((uint)RMOUSE);
		break;
	}
	// Check if mouse has been moved.
	case WM_MOUSEMOVE:
	{
		i->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
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
