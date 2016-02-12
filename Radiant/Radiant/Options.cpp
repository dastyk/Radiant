#include "Options.h"
#include "System.h"


Options::Options()
{

}


Options::~Options()
{
}

const void Options::Init()
{
	_iniFile = System::GetFileHandler()->Loadini("config.ini");
	
	return void();
}

const void Options::Shutdown() 
{
	SAFE_SHUTDOWN(_iniFile);
	return void();
}

const bool Options::GetFullscreen()const
{
	return _iniFile->GetBoolean("Window", "Fullscreen", false);

}

const uint Options::GetScreenResolutionWidth()const
{
	return (uint)_iniFile->GetInteger("Screen", "Width", 800);
}

const uint Options::GetScreenResolutionHeight()const
{
	return (uint)_iniFile->GetInteger("Screen", "Height", 640);
}

const uint Options::GetRefreshRateNumerator()const
{
	return (uint)_iniFile->GetInteger("Screen", "RefreshRateNumerator", 60);
	
}

const uint Options::GetRefreshRateDenominator()const
{
	return (uint)_iniFile->GetInteger("Screen", "RefreshRateDenominator", 1);

	
}

const float Options::GetMasterVolume() const
{
	return (float)_iniFile->GetReal("Audio", "Master", 0.5);

}

const float Options::GetMusicVolume() const
{
	return (float)_iniFile->GetReal("Audio", "Music", 1);
	
}

const float Options::GetSoundEffectVolume() const
{
	return (float)_iniFile->GetReal("Audio", "SoundEffect", 1);

	
}

const bool Options::GetVsync()const
{
	return _iniFile->GetBoolean("Graphics", "Vsync", false);

	
}

const uint Options::GetFoV()const
{
	return (uint)_iniFile->GetInteger("Graphics", "FOV", 90);

}

const float Options::GetAspectRatio()const
{
	return (float)_iniFile->GetReal("Graphics", "AspectRatio", 1.25);

}

const float Options::GetViewDistance()const
{
	return (float)_iniFile->GetReal("Graphics", "ViewDistance", 1000.0);
	
}

const float Options::GetNearPlane() const
{
	return (float)_iniFile->GetReal("Graphics", "NearPlane", 0.01);
}

const void Options::SetFullscreen(bool full) const
{
	_iniFile->SetBoolean("Window", "Fullscreen", full);
}

const void Options::SetScreenResolutionWidth(uint width)const
{
	_iniFile->SetInteger("Screen", "Width", (long)width);
}


const void Options::SetScreenResolutionHeight(uint height)const
{
	_iniFile->SetInteger("Screen", "Height", (long)height);
}



const void Options::SetRefreshRateNumerator(uint val)const
{
	_iniFile->SetInteger("Screen", "RefreshRateNumerator", (long)val);
}


const void Options::SetRefreshRateDenominator(uint val)const
{
	_iniFile->SetInteger("Screen", "RefreshRateDenominator", (long)val);
}



const void Options::SetMasterVolume(float vol)const
{
	_iniFile->SetReal("Audio", "Master", (double)vol);
}


const void Options::SetMusicVolume(float vol)const
{
	_iniFile->SetReal("Audio", "Music", (double)vol);
}


const void Options::SetSoundEffectVolume(float vol)const
{
	_iniFile->SetReal("Audio", "SoundEffects", (double)vol);
}



const void Options::SetVsync(bool vsync)const
{
	_iniFile->SetBoolean("Graphics", "Vsync", vsync);
}



const void Options::SetFoV(uint fov)const
{
	_iniFile->SetInteger("Graphics", "FOV", (long)fov);
}


const void Options::SetAspectRatio(float r)const
{
	_iniFile->SetReal("Graphics", "AspectRatio", (double)r);
}



const void Options::SetViewDistance(float dist)const
{
	_iniFile->SetReal("Graphics", "ViewDistance", (double)dist);
}


const void Options::SetNearPlane(float dist)const
{
	_iniFile->SetReal("Graphics", "NearPlane", (double)dist);
}



string Options::Get(string section, string name, string default_value)
{
	return _iniFile->Get(section, name, default_value);
}

const void Options::Set(string section, string name, string value)
{
	_iniFile->Set(section, name, value);
}
