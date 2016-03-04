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

const void FileHandler::DumpToFile(const std::string& line) const
{
	_fileDumper->DumpToFile(line);
}

void FileHandler::FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
	{
		throw ErrorMsg(6000005, L"Error loading wav file in function FindChunk");
	}

	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	while (hr == S_OK)
	{
		DWORD dwRead;
		if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());
			break;

		default:
			if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
			{
				throw ErrorMsg(6000006, L"Error loading wav file in function FindChunk");
			}
		}

		dwOffset += sizeof(DWORD) * 2;

		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize)
		{
			throw ErrorMsg(6000007, L"Error loading wav file in function FindChunk, too much data loaded");
		}

	}

	return;

}

void FileHandler::ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
	{
		throw ErrorMsg(6000008, L"Error loading wav file in function ReadChunkData");
	}

	DWORD dwRead;
	if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
	{
		throw ErrorMsg(6000009, L"Error loading wav file in function ReadChunkData");
	}

	return;
}

unsigned long FileHandler::LoadWav(TCHAR* fileName, WAVEFORMATEXTENSIBLE &wfx, XAUDIO2_BUFFER &buffer)
{
	HRESULT result = S_OK;
	DWORD errorInfo;

	// Open the file
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		errorInfo = GetLastError();
		throw ErrorMsg(6000010, L"Error loading wav file in function LoadWav");
	}

	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
	{
		throw ErrorMsg(6000011, L"Error loading wav file in function LoadWav");
	}


	DWORD dwChunkSize;
	DWORD dwChunkPosition;
	//check the file type, should be fourccWAVE or 'XWMA'
	FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);

	DWORD filetype;
	ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);

	if (filetype != fourccWAVE)
	{
		throw ErrorMsg(6000012, L"Error loading wav file in function LoadWav, problem with file type format, should be fourccWAVE");
	}


	FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);

	ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);



	//fill out the audio data buffer with the contents of the fourccDATA chunk
	FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);

	BYTE * pDataBuffer = new BYTE[dwChunkSize];
	ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

	buffer.AudioBytes = dwChunkSize;  //buffer containing audio data
	buffer.pAudioData = pDataBuffer;  //size of the audio buffer in bytes
	buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

	return dwChunkSize;
}
