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
#include "ini.h"
#include "FileDumper.h"
class FileHandler
{
public:
	FileHandler();
	~FileHandler();

	const void Init();
	const void Shutdown();

	Mesh* LoadModel(std::string filename)const;
	ini* Loadini(std::string path)const;
	const void DumpToFile(std::string line)const;

private:
	ModelLoader* _modelLoader;
	FileDumper* _fileDumper;
};

#endif