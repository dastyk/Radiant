#ifndef AUDIO_H
#define AUDIO_H

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
#include "ClassIncludes.h"
#include "VoiceCallback.h"

#include <mutex>
#include <thread>
#include <xaudio2.h>

#pragma comment(lib, "Xaudio2.lib")

struct SourceVoiceData
{
	IXAudio2SourceVoice* pSourceVoice;
	XAUDIO2_BUFFER Buffer;
	VoiceCallback* voiceCallback;

	std::wstring filename;
	bool active;
	time_t lastUsed;

};

struct LocatedVoice
{
	int index;
	bool correct;
};

class Audio
{
private:

	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasterVoice;
	IXAudio2SourceVoice* pMusicVoice;
	XAUDIO2_BUFFER musicBuffer;

	std::mutex mtx;
	SourceVoiceData voices[20];

	HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset);
	HRESULT LoadBuffer(TCHAR* fileName, WAVEFORMATEXTENSIBLE &wfx, XAUDIO2_BUFFER &buffer);
	void FreeMemory(IXAudio2SourceVoice* pSourceVoice, VoiceCallback* voiceCallback, XAUDIO2_BUFFER buffer);
	LocatedVoice ChooseVoice(wchar_t* filename);
	void FindFinishedVoices();

	bool finished;
	float masterVolume;
	float soundEffectsVolume;
	float musicVolume;

	wchar_t directory[200];

public:
	Audio(wchar_t directoryPath[200]);
	virtual ~Audio();

	HRESULT Play3DSound();
	HRESULT PlayGlobalSound(wchar_t* filename, float volume);
	HRESULT PlayBGMusic(wchar_t* filename, float volume);


};

#endif

