#ifndef _CPUTTIMER_H_
#define _CPUTTIMER_H_

#pragma once
#include "Utils.h"
#include "Timer.h"
#include "General.h"
#include <map>

class CPUTimer
{

public:
	CPUTimer();
	const void TimeStart(const std::string& name);
	const void TimeEnd(const std::string& name);
	const float GetTime(const std::string& name);
	const float GetTime();
	const float GetAVGTPF(const std::string& name);
private:

	struct ProfileData
	{
		BOOL QueryStarted;
		BOOL QueryFinished;

		Timer* timer;
		float _frameTime = 0.0f;
		float _ltime = 0.0f;
		float _timeElapsed = 0.0f;
		uint _frameCount = 0;
		ProfileData() : QueryStarted(FALSE), QueryFinished(FALSE)
		{
			timer = nullptr;
		}
		~ProfileData()
		{
			delete timer;
		}
	};

	typedef std::map<std::string, ProfileData> ProfileMap;

	ProfileMap profiles;
	UINT64 currFrame = 0;
	Timer _timer;

};

#endif