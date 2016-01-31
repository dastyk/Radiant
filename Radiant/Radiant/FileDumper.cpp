#include "FileDumper.h"



FileDumper::FileDumper()
{
	file.open("dumpfile.log", std::ios::trunc | std::ios::out);
	if (!file.is_open())
		throw ErrorMsg(6000014U, L"Could not open dumpfile.");
}


FileDumper::~FileDumper()
{
	file.close();
}

const void FileDumper::DumpToFile(std::string & line)
{

	_lines.push_back(line);

	return void();
}

const void FileDumper::Write()
{
	std::string line = _lines.back();
	_lines.pop_back();
	file << line << "n";
	return void();
}
