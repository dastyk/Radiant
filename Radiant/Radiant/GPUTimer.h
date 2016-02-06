#ifndef _GPUTIMER_H_
#define _GPUTIMER_H_


#pragma once
#include <d3d11.h>
#include "General.h"
#include "Timer.h"
#include "Utils.h"
#include <map>

class GPUTimer
{
public:
	const void TimeStart(const std::string& name);
	const void TimeEnd(const std::string& name);
	const void GetTime();

private:

    // Constants
    static const UINT64 QueryLatency = 5;

	struct ProfileData
	{
		ID3D11Query* DisjointQuery[QueryLatency];
		ID3D11Query* TimestampStartQuery[QueryLatency];
		ID3D11Query* TimestampEndQuery[QueryLatency];
		BOOL QueryStarted;
		BOOL QueryFinished;

		ProfileData() : QueryStarted(FALSE), QueryFinished(FALSE)
		{
			ZeroMemory(DisjointQuery, sizeof(ID3D11Query) * QueryLatency);
			ZeroMemory(TimestampStartQuery, sizeof(ID3D11Query) * QueryLatency);
			ZeroMemory(TimestampEndQuery, sizeof(ID3D11Query) * QueryLatency);
		}
		~ProfileData()
		{
			for (uint i = 0; i < QueryLatency; i++)
			{
				SAFE_RELEASE(DisjointQuery[i]);
				SAFE_RELEASE(TimestampStartQuery[i]);
				SAFE_RELEASE(TimestampEndQuery[i]);
			}
		}
	};

    typedef std::map<std::string, ProfileData> ProfileMap;

    ProfileMap profiles;
    UINT64 currFrame = 0;


    Timer timer;
};

#endif