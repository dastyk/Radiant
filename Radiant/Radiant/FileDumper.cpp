#include "FileDumper.h"

static unsigned __stdcall ThreadStaticEntryPoint(void * pThis)
{
	FileDumper* p = (FileDumper*)pThis;


	p->Write();

	return 1;
}

FileDumper::FileDumper()
{
	_file.open("dumpfile.log", std::ios::trunc | std::ios::out);
	if (!_file.is_open())
		throw ErrorMsg(6000014U, L"Could not open dumpfile.");
	_file.close();

	_writeMutex = CreateMutex(NULL, FALSE, NULL);

	_exit = false;

	_threadHandle = (HANDLE)_beginthreadex(NULL,
		0,            
		ThreadStaticEntryPoint,
		this,           
		0, 
		&_threadAddress);

	//_beginthread(write, 0, (void*)this);
}


FileDumper::~FileDumper()
{
	_exit = true;
	WaitForSingleObject(_threadHandle, INFINITE);
	CloseHandle(_writeMutex);

}

const void FileDumper::DumpToFile(std::string line)
{

	WaitForSingleObject(_writeMutex, INFINITE);
	_lines.push_back(line);
	ReleaseMutex(_writeMutex);

	
	return void();
}



const void FileDumper::Write()
{
	_file.open("dumpfile.log", std::ios::ate | std::ios::out | std::ios_base::app);
	while (!_exit)
	{
		if (_lines.size() != 0)
		{
			WaitForSingleObject(_writeMutex, INFINITE);

			std::string line = _lines.back();
			_lines.pop_back();
			_file << line << "\n";

			ReleaseMutex(_writeMutex);
		}
	}

	_file.close();
	return void();
}
