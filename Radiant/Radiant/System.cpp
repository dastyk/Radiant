#include "System.h"


#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // Memoryleak detection.
																  //_crtBreakAlloc = 188;
#endif

	// Create and get the system instance
	try
	{	
		System::CreateInstance();

		System* s = System::GetInstance();

		// Initialize the system
		s->Init();

		// Start the application
		s->StartUp();
	}
	catch (ErrorMsg& err)
	{
		err.Print();
		// Non fixable error occurred.
		System::DeleteInstance();
		return err.errorMsg;
	}
	catch (FinishMsg& fin)
	{
		// The application exited normally
		fin.Print();
		System::DeleteInstance();
		return fin.finishMsg;
	}
	return 0;
}

System* System::_instance = nullptr;



System::System()
{
	_windowHandler = nullptr;
	_inputInst = nullptr;
}


System::~System()
{
	
}

void System::CreateInstance()
{
	if(!_instance)
	{
		_instance = new System();
		if(!_instance) throw ErrorMsg(1000001, L"Filed to create system instance.");
	}
}

System * System::GetInstance()
{
	if (!_instance) throw ErrorMsg(1000002, L"Failed to get system instance.");
	return _instance;
}

void System::DeleteInstance()
{
	if (_instance)
	{
		_instance->ShutDown();
		delete _instance;
		_instance = nullptr;
	}
}

WindowHandler* System::GetWindowHandler() const
{
	if (!_windowHandler)
		throw ErrorMsg(1000005, L"No instance of the window handler.");
	return _windowHandler;
}

Input* System::GetInput() const
{
	if (!_inputInst)
		throw ErrorMsg(1000006, L"No instance of the input class.");

	return _inputInst;
}

Graphics* System::GetGraphics()
{
	if (!_graphics)
		throw ErrorMsg( 1000008, L"No instance of the graphics class." );

	return _graphics;
}

void System::Init()
{
	_CreateInputInst();
	// Create the window instance
	_CreateWindowHandler();

	// Create the input instance

	// Create the Graphics instance
	//_CreateGraphicsInst(HWND());
	_graphics = new Graphics();
	_graphics->Start( _windowHandler->GetHWnd(), _windowHandler->GetWindowWidth(), _windowHandler->GetWindowHeight() );

	///....s
}

void System::StartUp()
{
	_windowHandler->StartUp();
}

void System::ShutDown()
{
	if (_windowHandler)
	{
		_windowHandler->ShutDown();
		delete _windowHandler;
		_windowHandler = nullptr;
	}
	if (_inputInst)
	{
		_inputInst->ShutDown();
		delete _inputInst;
		_inputInst = nullptr;
	}
	if (_graphics)
	{
		_graphics->Shutdown();
		delete _graphics;
		_graphics = nullptr;
	}
}

void System::_CreateWindowHandler()
{
	try { _windowHandler = new WindowHandler; }
	catch (std::exception & e) { throw ErrorMsg(1000003, L"Failed to create window handler."); }

	_windowHandler->Init();

}

void System::_CreateInputInst()
{
	try { _inputInst = new Input; }
	catch (std::exception & e) { throw ErrorMsg(1000004, L"Failed to create input instance."); }

	_inputInst->Init();
}
