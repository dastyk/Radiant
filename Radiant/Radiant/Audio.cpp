#include "Audio.h"
#include "System.h"

Audio::Audio()
{
	HRESULT result;

	result = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	result = pXAudio2->CreateMasteringVoice(&pMasterVoice);
	pMusicVoice = nullptr;
	ZeroMemory(&musicBuffer, sizeof(musicBuffer));
	musicBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	masterVolume = System::GetOptions()->GetMasterVolume();
	musicVolume = System::GetOptions()->GetMusicVolume();
	soundEffectsVolume = System::GetOptions()->GetSoundEffectVolume();

	finished = false;
	for (int i = 0; i < MAX_SOUNDS; i++)
	{
		voices[i].active = false;
		time(&voices[i].lastUsed);
		ZeroMemory(&voices[i].Buffer, sizeof(voices[i].Buffer));
		voices[i].wfx = { 0 };
		voices[i].filename.clear();
	}

	std::thread(&Audio::FindFinishedVoices, this).detach();
}

Audio::~Audio()
{
	mtx.lock();
	finished = true;
	Sleep(100); // to give the thread enough time to close down
	mtx.unlock();

	for (int i = 0; i < MAX_SOUNDS; i++)
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

LocatedVoice Audio::ChooseVoice(wchar_t* filename)
{
	LocatedVoice rtnValue;
	bool foundReplacable;
	foundReplacable = false;
	int replacable;

	time_t current;
	time(&current);
	double temp;
	double oldestTime = 0;

	for (int i = 0; i < MAX_SOUNDS; i++)
	{

		if (wcscmp(voices[i].filename.c_str(), filename) == 0 && voices[i].active == false) // We found an already loaded and ready voice
		{
			rtnValue.index = i;
			rtnValue.type = 1;
			rtnValue.loadedData = -1;
			return rtnValue;
		}
		else if (wcscmp(voices[i].filename.c_str(), filename) == 0 && voices[i].active == true) // We found an already loaded but active voice
		{
			rtnValue.loadedData = i;
		}
		else if (wcscmp(voices[i].filename.c_str(), L"") == 0) // We found an unused voice
		{
			rtnValue.index = i;
			rtnValue.type = 0;
			rtnValue.loadedData = -1;
			return rtnValue;
		}
		else if (voices[i].active == false) // We found a replacable voice
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
		rtnValue.type = -1;
		return rtnValue;
	}
	else
	{
		rtnValue.index = -1;
		rtnValue.type = 1000;
		rtnValue.loadedData = -1;
		return rtnValue;
	}

}

void Audio::FindFinishedVoices()
{

	while (true)
	{
		mtx.lock();

		if (finished == true) // Engine is shutting down
		{
			mtx.unlock();
			return;
		}

		for (int i = 0; i < MAX_SOUNDS; i++)
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
		Sleep(100);
	}
}

void Audio::PlaySoundEffect(wchar_t* filename, float volume)
{
	masterVolume = System::GetOptions()->GetMasterVolume();
	musicVolume = System::GetOptions()->GetMusicVolume();
	soundEffectsVolume = System::GetOptions()->GetSoundEffectVolume();

	//std::thread(&Audio::LoadAndPlaySoundEffect, this, filename, volume).detach();
	LoadAndPlaySoundEffect(filename, volume);

}

void Audio::LoadAndPlaySoundEffect(wchar_t* filename, float volume)
{
	mtx.lock();

	wchar_t filePath[300] = L"";

	wcscat_s(filePath, System::GetInstance()->GetDirectory());
	wcscat_s(filePath, L"\\Audio\\SoundEffects\\");
	wcscat_s(filePath, filename);

	HRESULT hr = S_OK;
	LocatedVoice temp;
	temp.loadedData = -1;
	WAVEFORMATEXTENSIBLE wfx = { 0 };
	XAUDIO2_BUFFER testtemp = { 0 };

	temp = ChooseVoice(filename);

	if (temp.index < 0) // We couldn't fit the sound in somewhere... so whatever
	{
		mtx.unlock();
		return;
	}

	if (temp.type == 1) // Sound already loaded so just play it
	{
		voices[temp.index].pSourceVoice->SetVolume(masterVolume * soundEffectsVolume * volume);

		if (FAILED(hr = voices[temp.index].pSourceVoice->SubmitSourceBuffer(&voices[temp.index].Buffer)))
		{
			mtx.unlock();
	//		throw ErrorMsg(9000001, L"Failed to submit audio source buffer when working with the file " + voices[temp.index].filename);

			return;
		}

		if (FAILED(hr = voices[temp.index].pSourceVoice->Start(0)))
		{
			mtx.unlock();
//			throw ErrorMsg(9000002, L"Failed to start sound effect " + voices[temp.index].filename);

			return;
		}

		voices[temp.index].active = true;

		mtx.unlock();
		return;
	}
	else if (temp.type == 0) // Unused
	{
		voices[temp.index].voiceCallback = new VoiceCallback;

		if (temp.loadedData > -1) // found one but it's active, so copy over the data and use it instead of loading it again
		{
			voices[temp.index].wfx = voices[temp.loadedData].wfx;
			voices[temp.index].Buffer = voices[temp.loadedData].Buffer;
			voices[temp.index].bufferLength = voices[temp.loadedData].bufferLength;
			BYTE* tempArr = new BYTE[voices[temp.loadedData].bufferLength];

			for (uint i = 0; i < voices[temp.loadedData].bufferLength; i++)
			{
				tempArr[i] = voices[temp.loadedData].Buffer.pAudioData[i];

			}

			voices[temp.index].Buffer.pAudioData = tempArr;
		}
		else
		{
			voices[temp.index].bufferLength = System::GetInstance()->GetFileHandler()->LoadWav(filePath, voices[temp.index].wfx, voices[temp.index].Buffer);
		}


		if (FAILED(hr = pXAudio2->CreateSourceVoice(&voices[temp.index].pSourceVoice, (WAVEFORMATEX*)&voices[temp.index].wfx,
			0, XAUDIO2_DEFAULT_FREQ_RATIO, voices[temp.index].voiceCallback, NULL, NULL)))
		{
			mtx.unlock();
		//	throw ErrorMsg(9000003, L"Failed to create source voice for sound effect yo" + voices[temp.index].filename);

			return;
		}

		if (FAILED(hr = voices[temp.index].pSourceVoice->SubmitSourceBuffer(&voices[temp.index].Buffer)))
		{
			mtx.unlock();
	//		throw ErrorMsg(9000001, L"Failed to submit audio source buffer when working with the file " + voices[temp.index].filename);
		
			return;
		}

		voices[temp.index].pSourceVoice->SetVolume(masterVolume * soundEffectsVolume * volume);

		if (FAILED(hr = voices[temp.index].pSourceVoice->Start(0)))
		{
			mtx.unlock();
	//		throw ErrorMsg(9000002, L"Failed to start sound effect " + voices[temp.index].filename);
			
			return;
		}

		time(&voices[temp.index].lastUsed);
		voices[temp.index].active = true;
		voices[temp.index].filename.append(filename);

		mtx.unlock();
		return;
	}
	else if (temp.type == -1) // Destroying and reusing an earlier used voice that is not active
	{

		voices[temp.index].pSourceVoice->DestroyVoice();
		delete voices[temp.index].Buffer.pAudioData;
		voices[temp.index].filename.clear();
		voices[temp.index].wfx = { 0 };

		if (temp.loadedData > -1) // found one but it's active, so copy over the data and use it instead of loading it again
		{
			ZeroMemory(&voices[temp.index].wfx, sizeof(voices[temp.index].wfx));
			ZeroMemory(&voices[temp.index].Buffer, sizeof(voices[temp.index].Buffer));

			voices[temp.index].wfx = voices[temp.loadedData].wfx;
			voices[temp.index].Buffer = voices[temp.loadedData].Buffer;
			voices[temp.index].bufferLength = voices[temp.loadedData].bufferLength;
			BYTE* tempArr = new BYTE[voices[temp.loadedData].bufferLength];

			for (uint i = 0; i < voices[temp.loadedData].bufferLength; i++)
			{
				tempArr[i] = voices[temp.loadedData].Buffer.pAudioData[i];

			}

			voices[temp.index].Buffer.pAudioData = tempArr;
		}
		else
		{
			voices[temp.index].bufferLength = System::GetInstance()->GetFileHandler()->LoadWav(filePath, voices[temp.index].wfx, voices[temp.index].Buffer);
		}


		if (FAILED(hr = pXAudio2->CreateSourceVoice(&voices[temp.index].pSourceVoice, (WAVEFORMATEX*)&voices[temp.index].wfx,
			0, XAUDIO2_DEFAULT_FREQ_RATIO, voices[temp.index].voiceCallback, NULL, NULL)))
		{
			mtx.unlock();
			//throw ErrorMsg(9000003, L"Failed to create source voice for sound effect " + voices[temp.index].filename);

			return;
		}

		if (FAILED(hr = voices[temp.index].pSourceVoice->SubmitSourceBuffer(&voices[temp.index].Buffer)))
		{
			mtx.unlock();
			//throw ErrorMsg(9000001, L"Failed to submit audio source buffer when working with the file " + voices[temp.index].filename);

			return;
		}

		voices[temp.index].pSourceVoice->SetVolume(masterVolume * soundEffectsVolume * volume);

		if (FAILED(hr = voices[temp.index].pSourceVoice->Start(0)))
		{
			mtx.unlock();
			//throw ErrorMsg(9000002, L"Failed to start sound effect " + voices[temp.index].filename);

			return;
		}

		time(&voices[temp.index].lastUsed);
		voices[temp.index].active = true;
		voices[temp.index].filename.append(filename);

		mtx.unlock();
		return;
	}
	
	

	return; //Umm?
}

void Audio::PlayBGMusic(wchar_t * filename, float volume)
{
	masterVolume = System::GetOptions()->GetMasterVolume();
	musicVolume = System::GetOptions()->GetMusicVolume();
	soundEffectsVolume = System::GetOptions()->GetSoundEffectVolume();

	if (pMusicVoice != nullptr)
	{
		pMusicVoice->DestroyVoice();
		delete musicBuffer.pAudioData;
		delete musicCallback;
	}

	wchar_t filePath[300] = L"";

	wcscat_s(filePath, System::GetInstance()->GetDirectory());
	wcscat_s(filePath, L"\\Audio\\BGMusic\\");
	wcscat_s(filePath, filename);

	HRESULT hr = S_OK;
	WAVEFORMATEXTENSIBLE wfx = { 0 };

	System::GetInstance()->GetFileHandler()->LoadWav(filePath, wfx, musicBuffer);

	musicCallback = new VoiceCallback;
	if (FAILED(hr = pXAudio2->CreateSourceVoice(&pMusicVoice, (WAVEFORMATEX*)&wfx,
		0, XAUDIO2_DEFAULT_FREQ_RATIO, musicCallback, NULL, NULL)))
	{
	//	throw ErrorMsg(9000004, L"Failed to create source voice for background music");
		return;
	}

	if (FAILED(hr = pMusicVoice->SubmitSourceBuffer(&musicBuffer)))
	{
	//	throw ErrorMsg(9000005, L"Failed to submit audio source buffer for background music");
		return;
	}

	hr = pMusicVoice->SetVolume(masterVolume * musicVolume * volume);

	if (FAILED(hr = pMusicVoice->Start(0)))
	{
	//	throw ErrorMsg(9000006, L"Failed to start background music");
		return;
	}

	return;
}
