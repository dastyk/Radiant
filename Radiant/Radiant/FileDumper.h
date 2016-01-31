#ifndef _FILEDUMPER_H_
#define _FILEDUMPER_H_

#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "General.h"
#include <thread>
#include <process.h>



class FileDumper
{
public:
	FileDumper();
	~FileDumper();


	const void DumpToFile(std::string line);

	const void Write();

private:
	std::vector<std::string> _lines;
	std::ofstream _file;

	HANDLE _threadHandle;
	uint _threadAddress;
	HANDLE  _writeMutex;                // mutex for writing to file

	bool _exit;
};


#endif