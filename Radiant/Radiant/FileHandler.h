#ifndef _FILEHANDLER_H_
#define _FILEHANDLER_H_

#pragma once
//////////////
// Includes //
//////////////


////////////////////
// Local Includes //
////////////////////
#include "General.h"
#include "ModelLoader.h"
#include "INIReader.h"

class FileHandler
{
public:
	FileHandler();
	~FileHandler();

	const void Init();
	const void Shutdown();

	const void LoadModel()const;
	const INIReader LoadIni(std::string path)const;
private:
	ModelLoader* _modelLoader;
};

#endif