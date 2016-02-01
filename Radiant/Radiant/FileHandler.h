#ifndef _FILEHANDLER_H_
#define _FILEHANDLER_H_

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

#pragma once
//////////////
// Includes //
//////////////
#include <xaudio2.h>

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
	const void DumpToFile(const std::string& line)const;


	unsigned long LoadWav(TCHAR* fileName, WAVEFORMATEXTENSIBLE &wfx, XAUDIO2_BUFFER &buffer); //return HRESULT_FROM_WIN32(GetLastError()); is replaced with throw error, hresult could maybe be extracted and thrown instead
private:
	ModelLoader* _modelLoader;
	FileDumper* _fileDumper;

	void FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition); //return HRESULT_FROM_WIN32(GetLastError()); is replaced with throw error, hresult could maybe be extracted and thrown instead
	void ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset); //return HRESULT_FROM_WIN32(GetLastError()); is replaced with throw error, hresult could maybe be extracted and thrown instead
};

#endif