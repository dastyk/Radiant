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
	const void TimeStart(const std::string& name);
	const void TimeEnd(const std::string& name);
	const void GetTime();

private:

	struct ProfileData
	{
		BOOL QueryStarted;
		BOOL QueryFinished;

		Timer* timer;

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

};

#endif