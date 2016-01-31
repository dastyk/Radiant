#include "FileHandler.h"



FileHandler::FileHandler()
{
	_modelLoader = nullptr;
	_fileDumper = nullptr;
}


FileHandler::~FileHandler()
{
}

const void FileHandler::Init()
{
	try { _modelLoader = new ModelLoader; }
	catch (std::exception& e) { e; throw ErrorMsg(6000001, L"Failed to create modelloader."); }
	
	try { _fileDumper = new FileDumper; }
	catch (std::exception& e) { e; throw ErrorMsg(6000015, L"Failed to create filedumper."); }

	return void();
}

const void FileHandler::Shutdown()
{
	SAFE_DELETE(_modelLoader);
	SAFE_DELETE(_fileDumper);
	return void();
}

Mesh* FileHandler::LoadModel(std::string filename) const
{
	return _modelLoader->LoadModel(filename);
}

ini* FileHandler::Loadini(std::string path) const
{
	ini* out = new ini(path);
	out->Init();
	return out;
}

const void FileHandler::DumpToFile(std::string line) const
{
	_fileDumper->DumpToFile(line);
	return void();
}
