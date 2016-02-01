#ifndef _AUDIO_H_
#define _AUDIO_H_

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

#define MAX_SOUNDS 20

#pragma once
//////////////
// Includes //
//////////////
#include <mutex>
#include <thread>
#include <xaudio2.h>

////////////////////
// Local Includes //
////////////////////
#include "General.h"
#include "VoiceCallback.h"
#include "Options.h"

#pragma comment(lib, "Xaudio2.lib") // Should be added somewhere else probably

struct SourceVoiceData
{
	IXAudio2SourceVoice* pSourceVoice;
	XAUDIO2_BUFFER Buffer;
	WAVEFORMATEXTENSIBLE wfx;
	VoiceCallback* voiceCallback;

	unsigned long bufferLength;
	std::wstring filename;
	bool active;
	time_t lastUsed;

};

struct LocatedVoice
{
	int index;
	int type; // -1 = destroy and reuse, 0 = unused, 1 = loaded and not active
	int loadedData;
	
};

class Audio
{
private:
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasterVoice;
	IXAudio2SourceVoice* pMusicVoice;
	XAUDIO2_BUFFER musicBuffer;
	VoiceCallback* musicCallback;

	std::mutex mtx;
	SourceVoiceData voices[MAX_SOUNDS];

	LocatedVoice ChooseVoice(wchar_t* filename);
	void FindFinishedVoices();
	void LoadAndPlaySoundEffect(wchar_t* filename, float volume);

	bool finished;
	float masterVolume;
	float soundEffectsVolume;
	float musicVolume;

public:
	Audio();
	~Audio();

	void PlayBGMusic(wchar_t* filename, float volume);
	void PlaySoundEffect(wchar_t* filename, float volume);
};

#endif
