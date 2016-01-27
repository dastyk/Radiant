#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#pragma once
#include "General.h"

class Options
{
public:
	Options();
	~Options();


	const void Init();
	const void Shutdown();

	const bool GetFullscreen()const;

	const uint GetWindowWidth()const;
	const uint GetWindowHeight()const;

	const uint GetWindowPosX()const;
	const uint GetWindowPosY()const;

	const uint GetScreenResolutionWidth()const;
	const uint GetScreenResolutionHeight()const;

	const uint GetRefreshRateNumerator()const;
	const uint GetRefreshRateDenominator()const;

	const bool GetVsync()const;

	const uint GetFoV()const;
	const float GetAspectRatio()const;

	const uint GetViewDistance()const;

private:
	bool _fullscreen;

	uint _windowWidth;
	uint _windowHeight;

	uint _windowPosX;
	uint _windowPosY;

	uint _screenResolutionWidth;
	uint _screenResolutionHeight;

	uint _refreshRateNumerator;
	uint _refreshRateDenominator;

	bool _vsync;

	uint _fov;
	float _aspectRatio;

	uint _viewDistance;
};

#endif