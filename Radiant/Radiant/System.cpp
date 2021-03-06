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
	//_crtBreakAlloc = 1658;
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

	// The application exited normally
	//fin.Print();

	System::DeleteInstance();

	return 0;
}

System* System::_instance = nullptr;



System::System()
{
	_windowHandler = nullptr;
	_inputInst = nullptr;
	_graphicsInst = nullptr;
	_fileHandler = nullptr;
	_options = nullptr;
	_audio = nullptr;
	_directory = nullptr;
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
	SAFE_SHUTDOWN(_instance);
	}

WindowHandler* System::GetWindowHandler()
{
	WindowHandler* p = System::GetInstance()->_windowHandler;
	if (!p)
		throw ErrorMsg(1000004, L"No instance of the window handler.");
	return p;
}

Input* System::GetInput()
{
	Input* p = System::GetInstance()->_inputInst;
	if (!p)
		throw ErrorMsg(1000006, L"No instance of the input class.");

	return p;
}

Graphics * System::GetGraphics()
{
	Graphics* p = System::GetInstance()->_graphicsInst;
	if (!p)
		throw ErrorMsg(1000008, L"No instance of the graphic class.");

	return p;
}


FileHandler * System::GetFileHandler()
{
	FileHandler* p = System::GetInstance()->_fileHandler;
	if (!p)
		throw ErrorMsg(1000008, L"No instance of the file handler.");
	return p;
}
	
Options * System::GetOptions()
{
	Options* p = System::GetInstance()->_options;
	if (!p)
		throw ErrorMsg(10000013, L"No instance of the options class.");
	return p;
}

Audio * System::GetAudio()
{
	Audio* a = System::GetInstance()->_audio;
	if (!a)
		throw ErrorMsg(10000015, L"No instance of the audio class.");
	return a;
}

wchar_t* System::GetDirectory() const
{
	return _directory;
}

void System::Init()
{
	_CreateFileHandler();
	_CreateOptionsInst();
	_CreateInputInst();
	// Create the window instance
	_CreateWindowHandler();
	_CreateGraphicsInst();
	
	// Create the input instance

	// Create the Graphics instance
	//_CreateGraphicsInst(HWND());

	///....s

	_CreateAudioInst();

	_directory = new wchar_t[200];
	GetCurrentDirectory(200, _directory);
}

void System::StartUp()
{
	_windowHandler->StartUp();
}

void System::Shutdown()
{
	SAFE_SHUTDOWN(_windowHandler);
	SAFE_SHUTDOWN(_graphicsInst);
	SAFE_SHUTDOWN(_inputInst);
	SAFE_SHUTDOWN(_options);
	SAFE_SHUTDOWN(_fileHandler);


	SAFE_DELETE(_audio);
	SAFE_DELETE(_directory);
}

const void System::ToggleFullscreen()
{
	_inputInst->LockMouseToWindow(_options->GetFullscreen());
	_windowHandler->OnResize();
	_graphicsInst->ResizeSwapChain();
	return void();
}

void System::_CreateWindowHandler()
{
	try { _windowHandler = new WindowHandler; }
	catch (std::exception & e) { e;  throw ErrorMsg(1000003, L"Failed to create window handler."); }

	_windowHandler->Init();

}

void System::_CreateGraphicsInst()
{
	try { _graphicsInst = new Graphics; }
	catch (std::exception & e) { e;  throw ErrorMsg(1000007, L"Failed to create instance of graphic class."); }

	_graphicsInst->Init();
}

void System::_CreateInputInst()
{
	try { _inputInst = new Input; }
	catch (std::exception & e) { e; throw ErrorMsg(1000005, L"Failed to create instance of input class."); }

	_inputInst->Init();
}

void System::_CreateFileHandler()
{
	try { _fileHandler = new FileHandler; }
	catch (std::exception & e) { e; throw ErrorMsg(10000012, L"Failed to create instance of file handler."); }

	_fileHandler->Init();
}


void System::_CreateOptionsInst()
{
	try { _options = new Options; }
	catch (std::exception & e) { e; throw ErrorMsg(10000014, L"Failed to create instance of the options class."); }

	_options->Init();
}

void System::_CreateAudioInst()
{
	try { _audio = new Audio; }
	catch (std::exception & e) { e; throw ErrorMsg(10000016, L"Failed to create instance of the audio class."); }
}
