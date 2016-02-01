#include "Options.h"
#include "System.h"


Options::Options()
{
	_fullscreen = false;

	_windowWidth = 800;
	_windowHeight = 640;

	_windowPosX = 200;
	_windowPosY = 200;

	_screenResolutionWidth = 800;
	_screenResolutionHeight = 640;

	_refreshRateNumerator = 60;
	_refreshRateDenominator = 1;

	_vsync = false;

	_fov = 90;
	_aspectRatio = 1.25f;

	_viewDistance = 1000;
}


Options::~Options()
{
}

const void Options::Init()
{
	_iniFile = System::GetFileHandler()->Loadini("config.ini");
	_fullscreen = _iniFile->GetBoolean("Window", "Fullscreen", false);

	_windowWidth = (uint)_iniFile->GetInteger("Window", "Width", 800);
	_windowHeight = (uint)_iniFile->GetInteger("Window", "Hight", 640);

	_windowPosX = (uint)_iniFile->GetInteger("Window", "PosX", 200);
	_windowPosY = (uint)_iniFile->GetInteger("Window", "PosY", 200);

	_screenResolutionWidth = (uint)_iniFile->GetInteger("Screen", "Width", 800);
	_screenResolutionHeight = (uint)_iniFile->GetInteger("Screen", "Height", 640);

	_refreshRateNumerator = (uint)_iniFile->GetInteger("Screen", "RefreshRateNumerator", 60);
	_refreshRateDenominator = (uint)_iniFile->GetInteger("Screen", "RefreshRateDenominator", 1);

	_masterVolume = (float)_iniFile->GetReal("Audio", "Master", 0.5);
	_musicVolume = (float)_iniFile->GetReal("Audio", "Music", 1);
	_soundEffectVolume = (float)_iniFile->GetReal("Audio", "SoundEffect", 1);

	_vsync = _iniFile->GetBoolean("Graphics", "Vsync", false);

	_fov = (uint)_iniFile->GetInteger("Graphics", "FOV", 90);
	_aspectRatio = (float)_iniFile->GetReal("Graphics", "AspectRatio", 1.25);

	_viewDistance = (uint)_iniFile->GetInteger("Graphics", "ViewDistance", 1000);

	return void();
}

const void Options::Shutdown() 
{
	SAFE_SHUTDOWN(_iniFile);
	return void();
}

const bool Options::GetFullscreen()const
{
	return _fullscreen;
}

const uint Options::GetWindowWidth()const
{
	return _windowWidth;
}

const uint Options::GetWindowHeight()const
{
	return _windowHeight;
}

const uint Options::GetWindowPosX()const
{
	return _windowPosX;
}

const uint Options::GetWindowPosY()const
{
	return _windowPosY;
}

const uint Options::GetScreenResolutionWidth()const
{
	return _screenResolutionWidth;
}

const uint Options::GetScreenResolutionHeight()const
{
	return _screenResolutionHeight;
}

const uint Options::GetRefreshRateNumerator()const
{
	return _refreshRateNumerator;
}

const uint Options::GetRefreshRateDenominator()const
{
	return _refreshRateDenominator;
}

const float Options::GetMasterVolume() const
{
	return _masterVolume;
}

const float Options::GetMusicVolume() const
{
	return _musicVolume;
}

const float Options::GetSoundEffectVolume() const
{
	return _soundEffectVolume;
}

const bool Options::GetVsync()const
{
	return _vsync;
}

const uint Options::GetFoV()const
{
	return _fov;
}

const float Options::GetAspectRatio()const
{
	return _aspectRatio;
}

const uint Options::GetViewDistance()const
{
	return _viewDistance;
}

string Options::Get(string section, string name, string default_value)
{
	return _iniFile->Get(section, name, default_value);
}

const void Options::Set(string section, string name, string value)
{
	_iniFile->Set(section, name, value);
}
