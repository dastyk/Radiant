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
		MessageBoxW(0, err.errorText.c_str(), 0, 0);
		// Non fixable error occurred.
		System::DeleteInstance();
		return err.errorMsg;
	}
	catch (FinishMsg& fin)
	{
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

void System::Init()
{
	// Create the window instance
	_CreateWindowHandler();

	// Create the input instance
	_CreateInputInst();
	// Create the Graphics instance
	//_CreateGraphicsInst(HWND());

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
