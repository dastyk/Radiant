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

public:
	static void CreateInstance();
	static System* GetInstance();
	static void DeleteInstance();

	WindowHandler* GetWindowHandler()const;
	Input* GetInput()const;

	void Init();
	void StartUp();
	void ShutDown();
private:
	void _CreateWindowHandler();
	//void _CreateGraphicsInst(HWND hwnd);
	//void _CreateFactoryInst();
	void _CreateInputInst();

	//void _CreateAudioInst();

};

#endif
