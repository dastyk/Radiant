#include "CPUTimer.h"
#include "System.h"



const void CPUTimer::TimeStart(const std::string & name)
{
	ProfileData& profileData = profiles[name];
	if (profileData.QueryStarted != FALSE)
		return;
	if (profileData.QueryFinished != FALSE)
		return;

	if (profileData.timer == nullptr)
	{
		profileData.timer = new Timer;

	}
	profileData.timer->Reset();
	profileData.timer->Start();
	profileData.timer->Tick();
	profileData.QueryStarted = TRUE;
}

const void CPUTimer::TimeEnd(const std::string & name)
{
	auto i = profiles.find(name);
	if (i == profiles.end())
		return;
	ProfileData& profileData = i->second;
	_ASSERT(profileData.QueryStarted != FALSE);
	_ASSERT(profileData.QueryFinished == FALSE);

	profileData.timer->Tick();
	profileData.timer->Stop();

	profileData.QueryStarted = FALSE;
	profileData.QueryFinished = TRUE;
}

const void CPUTimer::GetTime()
{
	float total = 0.0f;

	// Iterate over all of the profiles
	ProfileMap::iterator iter;
	for (iter = profiles.begin(); iter != profiles.end(); ++iter)
	{
		ProfileData& profile = (*iter).second;
		if (profile.QueryFinished != FALSE)
		{

			profile.QueryFinished = FALSE;

			if (profile.timer != nullptr)
			{

				// Get the query data
				UINT64 startTime = 0;
			
				float time = profile.timer->TotalTime();
				
				total += time;
				string output = (*iter).first + ": " + to_string(time*1000.0f) + "ms";
				System::GetFileHandler()->DumpToFile(output);
			}
		}
	}
	string output = "Total time: " + to_string(total*1000.0f)+ " ms";
	System::GetFileHandler()->DumpToFile(output);
}
