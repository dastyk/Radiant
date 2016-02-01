#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(wchar_t directoryPath[200])
{
	HRESULT result;

	result = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	result = pXAudio2->CreateMasteringVoice(&pMasterVoice);
	pMusicVoice = nullptr;
	ZeroMemory(&musicBuffer, sizeof(musicBuffer));
	musicBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	masterVolume = 1;
	musicVolume = 0.5;
	soundEffectsVolume = 0.5;

	for (int i = 0; i < 20; i++)
	{
		voices[i].active = false;
		time(&voices[i].lastUsed);
		voices[i].filename = L"";
	}

	for (int i = 0; i < 200; i++)
		directory[i] = directoryPath[i];

	std::thread(&AudioPlayer::FindFinishedVoices, this).detach();
}

AudioPlayer::~AudioPlayer()
{
	mtx.lock();
	finished = true;
	mtx.unlock();

	for (int i = 0; i < 20; i++)
	{
		if (wcscmp(voices[i].filename.c_str(), L"") != 0)
		{
			voices[i].pSourceVoice->DestroyVoice();
			delete voices[i].Buffer.pAudioData;
			delete voices[i].voiceCallback;
		}
	}

	if (pMusicVoice != nullptr)
	{
		pMusicVoice->DestroyVoice();
		delete musicBuffer.pAudioData;
		delete musicCallback;
	}

	pXAudio2->Release();
}

HRESULT AudioPlayer::FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

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
				return HRESULT_FROM_WIN32(GetLastError());
		}

		dwOffset += sizeof(DWORD) * 2;

		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize) return S_FALSE;

	}

	return S_OK;

}

HRESULT AudioPlayer::ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());
	DWORD dwRead;
	if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
		hr = HRESULT_FROM_WIN32(GetLastError());
	return hr;
}

HRESULT AudioPlayer::LoadBuffer(TCHAR* fileName, WAVEFORMATEXTENSIBLE &wfx, XAUDIO2_BUFFER &buffer)
{
	HRESULT result = S_OK;

	// Open the file
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return HRESULT_FROM_WIN32(GetLastError());

	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());


	DWORD dwChunkSize;
	DWORD dwChunkPosition;
	//check the file type, should be fourccWAVE or 'XWMA'
	result = FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
	if (result != S_OK)
	{
		int error = 1;
	}
	DWORD filetype;
	result = ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
	if (result != S_OK)
	{
		int error = 1;
	}
	if (filetype != fourccWAVE)
		return S_FALSE;


	result = FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
	if (result != S_OK)
	{
		int error = 1;
	}
	result = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	if (result != S_OK)
	{
		int error = 1;
	}


	//fill out the audio data buffer with the contents of the fourccDATA chunk
	result = FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
	if (result != S_OK)
	{
		int error = 1;
	}
	BYTE * pDataBuffer = new BYTE[dwChunkSize];
	result = ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);
	if (result != S_OK)
	{
		int error = 1;
	}


	buffer.AudioBytes = dwChunkSize;  //buffer containing audio data
	buffer.pAudioData = pDataBuffer;  //size of the audio buffer in bytes
	buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

	return S_OK;
}

void AudioPlayer::FreeMemory(IXAudio2SourceVoice* pSourceVoice, VoiceCallback* voiceCallback, XAUDIO2_BUFFER buffer)
{
	WaitForSingleObjectEx(voiceCallback->hBufferEndEvent, INFINITE, TRUE);
	pSourceVoice->DestroyVoice();
	delete buffer.pAudioData;
	delete voiceCallback;
}

LocatedVoice AudioPlayer::ChooseVoice(wchar_t* filename)
{
	LocatedVoice rtnValue;
	bool foundReplacable;
	foundReplacable = false;
	int replacable;

	time_t current;
	time(&current);
	double temp;
	double oldestTime = 0;

	mtx.lock();

	for (int i = 0; i < 20; i++)
	{

		if (wcscmp(voices[i].filename.c_str(), filename) == 0 && voices[i].active == false) // We found an already loaded and ready voice
		{
			rtnValue.index = i;
			rtnValue.correct = true;
			mtx.unlock();
			return rtnValue;
		}
		else if (wcscmp(voices[i].filename.c_str(), L"") == 0) // We found an unused voice
		{
			rtnValue.index = i;
			rtnValue.correct = false;
			mtx.unlock();
			return rtnValue;
		}
		else if(voices[i].active == false) // We found a replacable voice
		{
			
			temp = difftime(current, voices[i].lastUsed);
			if (temp > oldestTime)
			{
				foundReplacable = true;
				oldestTime = temp;
				replacable = i;
			}

		}

	}

	if (foundReplacable)
	{
		rtnValue.index = replacable;
		rtnValue.correct = false;
		mtx.unlock();
		return rtnValue;
	}
	else
	{
		rtnValue.index = -1;
		rtnValue.correct = false;
		mtx.unlock();
		return rtnValue;
	}

}

void AudioPlayer::FindFinishedVoices()
{

	while (true)
	{
		mtx.lock();

		if (finished) // Engine is shutting down
		{
			mtx.unlock();
			return;
		}

		for (int i = 0; i < 20; i++)
		{
			if (voices[i].active)
			{
				if (WaitForSingleObjectEx(voices[i].voiceCallback->hBufferEndEvent, 1, TRUE) == WAIT_OBJECT_0)
				{
					voices[i].active = false;
				}
			}
		}

		mtx.unlock();
		Sleep(1000);
	}
}

HRESULT AudioPlayer::PlayGlobalSound(wchar_t* filename, float volume)
{
	wchar_t filePath[300] = L"";

	wcscat_s(filePath, directory);
	wcscat_s(filePath, L"\\Audio\\SoundEffects\\");
	wcscat_s(filePath, filename);

	HRESULT hr = S_OK;
	LocatedVoice temp;
	WAVEFORMATEXTENSIBLE wfx = { 0 };

	temp = ChooseVoice(filename);

	mtx.lock();

	if (temp.index == -1) // We couldn't fit the sound in somewhere... so whatever
		return S_OK;

	if (temp.correct) // Sound already loaded so just play it
	{
		voices[temp.index].pSourceVoice->SetVolume(masterVolume * soundEffectsVolume * volume);

		if (FAILED(hr = voices[temp.index].pSourceVoice->Start(0)))
		{
			mtx.unlock();
			return hr;
		}

		if (FAILED(hr = voices[temp.index].pSourceVoice->SubmitSourceBuffer(&voices[temp.index].Buffer)))
		{
			mtx.unlock();
			return hr;
		}

		mtx.unlock();
		return S_OK;
	}
	else
	{

		if (wcscmp(voices[temp.index].filename.c_str(), L"") == 0) // Unused
		{
			voices[temp.index].voiceCallback = new VoiceCallback;

			hr = LoadBuffer(filePath, wfx, voices[temp.index].Buffer);
			if (hr != S_OK)
			{
				mtx.unlock();
				return hr;
			}

			if (FAILED(hr = pXAudio2->CreateSourceVoice(&voices[temp.index].pSourceVoice, (WAVEFORMATEX*)&wfx,
				0, XAUDIO2_DEFAULT_FREQ_RATIO, voices[temp.index].voiceCallback, NULL, NULL)))
			{
				mtx.unlock();
				return hr;
			}

			if (FAILED(hr = voices[temp.index].pSourceVoice->SubmitSourceBuffer(&voices[temp.index].Buffer)))
			{
				mtx.unlock();
				return hr;
			}

			voices[temp.index].pSourceVoice->SetVolume(masterVolume * soundEffectsVolume * volume);

			if (FAILED(hr = voices[temp.index].pSourceVoice->Start(0)))
			{
				mtx.unlock();
				return hr;
			}

			time(&voices[temp.index].lastUsed);
			voices[temp.index].active = true;
			voices[temp.index].filename.append(filename);

			mtx.unlock();
			return S_OK;
		}

		voices[temp.index].pSourceVoice->DestroyVoice(); //Reusing
		delete voices[temp.index].Buffer.pAudioData;
		voices[temp.index].filename.clear();

		hr = LoadBuffer(filePath, wfx, voices[temp.index].Buffer);
		if (hr != S_OK)
		{
			mtx.unlock();
			return hr;
		}

		if (FAILED(hr = pXAudio2->CreateSourceVoice(&voices[temp.index].pSourceVoice, (WAVEFORMATEX*)&wfx,
			0, XAUDIO2_DEFAULT_FREQ_RATIO, voices[temp.index].voiceCallback, NULL, NULL)))
		{
			mtx.unlock();
			return hr;
		}

		if (FAILED(hr = voices[temp.index].pSourceVoice->SubmitSourceBuffer(&voices[temp.index].Buffer)))
		{
			mtx.unlock();
			return hr;
		}

		voices[temp.index].pSourceVoice->SetVolume(masterVolume * soundEffectsVolume * volume);

		if (FAILED(hr = voices[temp.index].pSourceVoice->Start(0)))
		{
			mtx.unlock();
			return hr;
		}

		time(&voices[temp.index].lastUsed);
		voices[temp.index].active = true;
		voices[temp.index].filename.append(filename);

		mtx.unlock();
		return S_OK;
	}

	return hr; //Umm?
}

HRESULT AudioPlayer::PlayBGMusic(wchar_t * filename, float volume)
{
	if (pMusicVoice != nullptr)
	{
		pMusicVoice->DestroyVoice();
		delete musicBuffer.pAudioData;
	}

	wchar_t filePath[300] = L"";

	wcscat_s(filePath, directory);
	wcscat_s(filePath, L"\\Audio\\BGMusic\\");
	wcscat_s(filePath, filename);

	HRESULT hr = S_OK;
	WAVEFORMATEXTENSIBLE wfx = { 0 };

	hr = LoadBuffer(filePath, wfx, musicBuffer);
	if (hr != S_OK)
	{
		return hr;
	}

	musicCallback = new VoiceCallback;
	if (FAILED(hr = pXAudio2->CreateSourceVoice(&pMusicVoice, (WAVEFORMATEX*)&wfx,
		0, XAUDIO2_DEFAULT_FREQ_RATIO, musicCallback, NULL, NULL)))
		return hr;

	if (FAILED(hr = pMusicVoice->SubmitSourceBuffer(&musicBuffer)))
		return hr;

	hr = pMusicVoice->SetVolume(masterVolume * musicVolume * volume);

	if (FAILED(hr = pMusicVoice->Start(0)))
		return hr;

	return S_OK;
}
