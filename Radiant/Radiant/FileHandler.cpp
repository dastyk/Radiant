#include "FileHandler.h"



FileHandler::FileHandler()
{
	_modelLoader = nullptr;
}


FileHandler::~FileHandler()
{
}

const void FileHandler::Init()
{
	try { _modelLoader = new ModelLoader; }
	catch (std::exception& e) { throw ErrorMsg(6000001, L"Failed to create modelloader."); }

	return void();
}

const void FileHandler::Shutdown()
{
	SAFE_DELETE(_modelLoader);
	return void();
}

const INIReader FileHandler::LoadIni(std::string path) const
{
	return INIReader(path);
}
