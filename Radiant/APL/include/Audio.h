#ifndef _STATIC_LIB_AUDIO_
#define __STATIC_LIB_AUDIO_

#pragma once
#include <sndfile.h>
#include <portaudio.h>
#include <map>
#include <vector>
#include <thread>

#include <stdint.h>
#pragma comment(lib, "libsndfile-1.lib")
#pragma comment(lib, "portaudio_x64.lib")

#define CallbackPrototype(x) \
int x(const void *inputBuffer, void *outputBuffer,\
unsigned long framesPerBuffer,\
const PaStreamCallbackTimeInfo* timeInfo,\
PaStreamCallbackFlags statusFlags,\
Audio::FileInfo fileInfo,\
void *userData)

#define FinishedCallbackPrototype(x) PaStreamCallbackResult x( void* userData )

class Audio
{
public:
	struct FileInfo
	{
		SF_INFO info;
		float* data;
	};
	typedef int(AudioProcessingCallback)(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		Audio::FileInfo fileInfo,
		void *userData);
	typedef PaStreamCallbackResult(AudioFinishedCallback)(void*);
	
	struct StreamInfo
	{
		FileInfo info;
		AudioProcessingCallback* callback;
		void* userData;
	};
	
private:
	
	static const void _WaitForStream(PaStream * stream, bool* running, AudioFinishedCallback* finishedCallback, void * userData);
	struct StreamData
	{
		StreamData() : stream(nullptr), threadRunning(nullptr), info(nullptr)
		{

		}
		PaStream* stream;
		std::thread waitThread;
		bool* threadRunning = false;
		StreamInfo* info;
	};
	Audio();
	~Audio();
	static Audio* _instance;



	std::map<uint32_t, StreamData> _streams;
	std::vector<FileInfo> _files;

	const void Error(const PaError err)const;
public:
	static void CreateInstance();
	static void ShutdownInstance();
	static Audio* GetInstance();

	FileInfo& ReadFile(char* file);
	const void CreateOutputStream(AudioProcessingCallback * callback, void * userData, Audio::FileInfo& fInfo, uint64_t bufferSize, uint32_t GUID);
	const void StartStream(uint32_t GUID, AudioFinishedCallback* finishedCallback = nullptr, void * userData = nullptr, bool async = true);
	const void StopStream(uint32_t GUID)const;

};


#endif