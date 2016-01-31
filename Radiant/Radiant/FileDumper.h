#ifndef _FILEDUMPER_H_
#define _FILEDUMPER_H_

#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "General.h"

class FileDumper
{
public:
	FileDumper();
	~FileDumper();


	const void DumpToFile(std::string& line);

private:
	const void Write();

private:
	std::vector<std::string> _lines;
	std::ofstream file;
};

#endif