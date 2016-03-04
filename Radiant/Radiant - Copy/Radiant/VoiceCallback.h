#ifndef VOICECALLBACK_H
#define VOICECALLBACK_H



#pragma once
#include <xaudio2.h>

#pragma comment(lib, "Xaudio2.lib")

class VoiceCallback : public IXAudio2VoiceCallback
{
public:
	HANDLE hBufferEndEvent;
	VoiceCallback() : hBufferEndEvent(CreateEvent(NULL, FALSE, FALSE, NULL)) {}
	~VoiceCallback() { CloseHandle(hBufferEndEvent); }

	//Called when the voice has just finished playing a contiguous audio stream.
	void __stdcall OnStreamEnd() { SetEvent(hBufferEndEvent); }

	//Unused methods are stubs
	void __stdcall OnVoiceProcessingPassEnd() { }
	void __stdcall OnVoiceProcessingPassStart(UINT32 SamplesRequired) {    }
	void __stdcall OnBufferEnd(void * pBufferContext) { }
	void __stdcall OnBufferStart(void * pBufferContext) {    }
	void __stdcall OnLoopEnd(void * pBufferContext) {    }
	void __stdcall OnVoiceError(void * pBufferContext, HRESULT Error) { }
};

#endif