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
	catch (std::exception& e) { e; throw ErrorMsg(6000001, L"Failed to create modelloader."); }

	return void();
}

const void FileHandler::Shutdown()
{
	SAFE_DELETE(_modelLoader);
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
