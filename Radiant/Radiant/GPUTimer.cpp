#include "GPUTimer.h"
#include "System.h"



GPUTimer::GPUTimer()
{
	timer.Start();
	timer.Reset();
}

const void GPUTimer::TimeStart(const std::string& name)
{
	auto device = System::GetGraphics()->GetDevice();
	auto context = System::GetGraphics()->GetDeviceContext();
	ProfileData& profileData = profiles[name];
	if (profileData.QueryStarted != FALSE)
		return;
	if (profileData.QueryFinished != FALSE)
		return;

	if (profileData.DisjointQuery[currFrame] == NULL)
	{
		HRESULT hr;
		// Create the queries
		D3D11_QUERY_DESC desc;
		desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
		desc.MiscFlags = 0;
		hr = device->CreateQuery(&desc, &profileData.DisjointQuery[currFrame]);
		if (FAILED(hr))
			throw ErrorMsg(1400002, L"Could not create D3D11_QUERY_TIMESTAMP_DISJOINT.");
		desc.Query = D3D11_QUERY_TIMESTAMP;
		hr = device->CreateQuery(&desc, &profileData.TimestampStartQuery[currFrame]);
		if (FAILED(hr))
			throw ErrorMsg(1400001, L"Could not create D3D11_QUERY_TIMESTAMP.");
		device->CreateQuery(&desc, &profileData.TimestampEndQuery[currFrame]);
		if (FAILED(hr))
			throw ErrorMsg(1400001, L"Could not create D3D11_QUERY_TIMESTAMP.");
	}

	// Start a disjoint query first
	context->Begin(profileData.DisjointQuery[currFrame]);

	// Insert the start timestamp    
	context->End(profileData.TimestampStartQuery[currFrame]);

	profileData.QueryStarted = TRUE;

}

const void GPUTimer::TimeEnd(const std::string& name)
{
	auto device = System::GetGraphics()->GetDevice();
	auto context = System::GetGraphics()->GetDeviceContext();
	auto i = profiles.find(name);
	if (i == profiles.end())
		return;
	ProfileData& profileData = i->second;
	_ASSERT(profileData.QueryStarted != FALSE);
	_ASSERT(profileData.QueryFinished == FALSE);

	// Insert the end timestamp    
	context->End(profileData.TimestampEndQuery[currFrame]);

	// End the disjoint query
	context->End(profileData.DisjointQuery[currFrame]);

	profileData.QueryStarted = FALSE;
	profileData.QueryFinished = TRUE;
}

const void GPUTimer::GetTime()
{
	float total = 0.0f;
	currFrame = (currFrame + 1) % QueryLatency;
	auto context = System::GetGraphics()->GetDeviceContext();
	XMMATRIX transform = XMMatrixTranslation(25.0f, 100.0f, 0.0f);

	float queryTime = 0.0f;

	// Iterate over all of the profiles
	ProfileMap::iterator iter;
	for (iter = profiles.begin(); iter != profiles.end(); ++iter)
	{
		ProfileData& profile = (*iter).second;
		if (profile.QueryFinished != FALSE)
		{

			profile.QueryFinished = FALSE;

			if (profile.DisjointQuery[currFrame] != NULL)
			{

				timer.Tick();

				// Get the query data
				UINT64 startTime = 0;
				while (context->GetData(profile.TimestampStartQuery[currFrame], &startTime, sizeof(startTime), 0) != S_OK);

				UINT64 endTime = 0;
				while (context->GetData(profile.TimestampEndQuery[currFrame], &endTime, sizeof(endTime), 0) != S_OK);

				D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
				while (context->GetData(profile.DisjointQuery[currFrame], &disjointData, sizeof(disjointData), 0) != S_OK);

				timer.Tick();

				queryTime += timer.DeltaTime();

				float time = 0.0f;
				if (disjointData.Disjoint == FALSE)
				{
					UINT64 delta = endTime - startTime;
					float frequency = static_cast<float>(disjointData.Frequency);
					time = (delta / frequency) * 1000.0f;
				}
				profile._frameTime += time;
				total += time;
				//string output = (*iter).first + ": " + to_string(time) + "ms";
				//System::GetFileHandler()->DumpToFile(output);
			}
		}
	}
	//string output = "Time spent waiting for queries: " + to_string(queryTime*1000.0f) + "ms. Total time: " + to_string(total);
	//System::GetFileHandler()->DumpToFile(output);
	
}

const float GPUTimer::GetAVGTPF(const std::string & name)
{
	timer.Tick();


	auto i = profiles.find(name);
	if (i == profiles.end())
		return 0.0f;
	ProfileData& profile = i->second;

	if (profile.DisjointQuery[currFrame] != NULL )
	{

		// Get the query data
		profile._frameCount++;
		float time = profile._ltime;
		if ((timer.TotalTime() - profile._timeElapsed) >= 1.0f)
		{
			profile._ltime = profile._frameTime / (float)profile._frameCount;
			profile._frameTime = 0.0f;
			profile._frameCount = 0;
			profile._timeElapsed += 1.0f;
		}


		return time;
	}

	return 0.0f;
}
