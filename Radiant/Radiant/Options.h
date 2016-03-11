#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#pragma once
#include "General.h"
#include <string>
#include "FileHandler.h"
using namespace std;
class Options
{
public:
	Options();
	~Options();


	const void Init();
	const void Options::Shutdown();

	const bool GetFullscreen()const;

	const uint GetScreenResolutionWidth()const;
	const uint GetScreenResolutionHeight()const;

	const uint GetRefreshRateNumerator()const;
	const uint GetRefreshRateDenominator()const;

	const float GetMasterVolume()const;
	const float GetMusicVolume()const;
	const float GetSoundEffectVolume()const;

	const bool GetVsync()const;
	const float GetGamma()const;


	const uint GetFoV()const;
	const float GetAspectRatio()const;

	const float GetViewDistance()const;
	const float GetNearPlane()const;

	const uint GetWeaponMode()const;
	const bool GetHardcoreMode()const;
	const uint GetDifficulty()const;

	const void SetFullscreen(bool full)const;

	const void SetScreenResolutionWidth(uint width)const;
	const void SetScreenResolutionHeight(uint height)const;

	const void SetRefreshRateNumerator(uint val)const;
	const void SetRefreshRateDenominator(uint val)const;

	const void SetMasterVolume(float vol)const;
	const void SetMusicVolume(float vol)const;
	const void SetSoundEffectVolume(float vol)const;

	const void SetVsync(bool vsync)const;
	const void SetGamma(float gamma)const;

	const void SetFoV(uint fov)const;
	const void SetAspectRatio(float r)const;

	const void SetViewDistance(float dist)const;
	const void SetNearPlane(float dist)const;
	
	const void SetWeaponMode(uint mode)const;
	const void SetHardcoreMode(bool mode)const;
	const void SetDifficulty(uint difficulty)const;


	string Get(string section, string name, string default_value);
	const void Options::Set(string section, string name, string value);
	ini* _iniFile;
};

#endif