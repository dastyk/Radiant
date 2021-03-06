#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#pragma once
//////////////
// Includes //
//////////////
#include <Windows.h>
#include <windowsx.h>

////////////////////
// Local Includes //
////////////////////
#include "General.h"
#include "WindowHandler.h"
#include "Input.h"
#include "Graphics.h"
#include "FileHandler.h"
#include "Options.h"
#include "Audio.h"
#include "GameState.h"

class System
{
private:
	System();
	System(const System& other);
	~System();
	System& operator=(const System& other);

private:
	static System* _instance;

	WindowHandler* _windowHandler;
	Input* _inputInst;
	Graphics* _graphicsInst;
	FileHandler* _fileHandler;
	Options* _options;
	Audio* _audio;

	wchar_t* _directory;
public:
	static void CreateInstance();
	static System* GetInstance();
	static void DeleteInstance();

	static WindowHandler* GetWindowHandler();
	static Input* GetInput();
	static Graphics* GetGraphics();
	static FileHandler* GetFileHandler();
	static Options* GetOptions();
	static Audio* GetAudio();

	wchar_t* GetDirectory()const;

	void Init();
	void StartUp();
	void Shutdown();

	const void ToggleFullscreen();

private:
	void _CreateWindowHandler();
	void _CreateGraphicsInst();
	//void _CreateFactoryInst();
	void _CreateInputInst();

	void _CreateFileHandler();
	void _CreateOptionsInst();
	void _CreateAudioInst();

};

#endif
