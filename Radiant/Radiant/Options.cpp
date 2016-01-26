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
	INIReader r = System::GetInstance()->GetFileHandler()->LoadIni("config.ini");
	if (!(r.ParseError() < 0))
	{
		_fullscreen = r.GetBoolean("Window", "Fullscreen", false);

		_windowWidth = (uint)r.GetInteger("Window", "Width", 800);
		_windowHeight = (uint)r.GetInteger("Window", "Hight", 640);

		_windowPosX = (uint)r.GetInteger("Window", "PosX", 200);
		_windowPosY = (uint)r.GetInteger("Window", "PosY", 200);

		_screenResolutionWidth = (uint)r.GetInteger("Screen", "Width", 800);
		_screenResolutionHeight = (uint)r.GetInteger("Screen", "Height", 640);

		_refreshRateNumerator = (uint)r.GetInteger("Screen", "RefreshRateNumerator", 60);
		_refreshRateDenominator = (uint)r.GetInteger("Screen", "RefreshRateDenominator", 1);

		_vsync = r.GetBoolean("Graphics", "Vsync", false);

		_fov = (uint)r.GetInteger("Graphics", "FOV", 90);
		_aspectRatio = (float)r.GetReal("Graphics", "AspectRatio", 1.25);

		_viewDistance = (uint)r.GetInteger("Graphics", "ViewDistance", 1000);
	}
	return void();
}

const void Options::Shutdown() 
{
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
