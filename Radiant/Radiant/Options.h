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

	const uint GetFoV()const;
	const float GetAspectRatio()const;

	const float GetViewDistance()const;
	const float GetNearPlane()const;

	const uint GetWeaponMode()const;

	const void Options::SetFullscreen(bool full)const;

	const void Options::SetScreenResolutionWidth(uint width)const;
	const void Options::SetScreenResolutionHeight(uint height)const;

	const void Options::SetRefreshRateNumerator(uint val)const;
	const void Options::SetRefreshRateDenominator(uint val)const;

	const void Options::SetMasterVolume(float vol)const;
	const void Options::SetMusicVolume(float vol)const;
	const void Options::SetSoundEffectVolume(float vol)const;

	const void Options::SetVsync(bool vsync)const;

	const void Options::SetFoV(uint fov)const;
	const void Options::SetAspectRatio(float r)const;

	const void Options::SetViewDistance(float dist)const;
	const void Options::SetNearPlane(float dist)const;
	
	const void SetWeaponMode(uint mode)const;


	string Get(string section, string name, string default_value);
	const void Options::Set(string section, string name, string value);
	ini* _iniFile;
};

#endif