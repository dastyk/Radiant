#include "GPUTimer.h"
#include "System.h"


GPUTimer::GPUTimer(const std::string& name) : _name(name)
{
	_start = nullptr;
	_stop = nullptr;
	_disjoint = nullptr;

	auto pDevice =  System::GetGraphics()->GetDevice();
	HRESULT hr;
	D3D11_QUERY_DESC desc;
	desc.Query = D3D11_QUERY_TIMESTAMP;
	desc.MiscFlags = 0;
	hr = pDevice->CreateQuery(&desc, &_start);
	if (FAILED(hr))
	{
		throw ErrorMsg(1400001, L"Could not create D3D11_QUERY_TIMESTAMP.");
	}

	hr = pDevice->CreateQuery(&desc, &_stop);
	if (FAILED(hr))
	{
		throw ErrorMsg(1400001, L"Could not create D3D11_QUERY_TIMESTAMP.");
	}

	desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	hr = pDevice->CreateQuery(&desc, &_disjoint);
	if (FAILED(hr))
	{
		throw ErrorMsg(1400002, L"Could not create D3D11_QUERY_TIMESTAMP_DISJOINT.");
	}
}


GPUTimer::~GPUTimer()
{
	SAFE_RELEASE(_start);
	SAFE_RELEASE(_stop);
	SAFE_RELEASE(_disjoint);
}

const void GPUTimer::TimeStart()
{

	auto pDeviceContext = System::GetGraphics()->GetDeviceContext();
	pDeviceContext->Begin(_disjoint);
	pDeviceContext->End(_start);

}

const void GPUTimer::TimeEnd()
{
	auto pDeviceContext = System::GetGraphics()->GetDeviceContext();
	pDeviceContext->End(_stop);
	pDeviceContext->End(_disjoint);
}

const double GPUTimer::GetTime()const
{
	auto pDeviceContext = System::GetGraphics()->GetDeviceContext();
	while (pDeviceContext->GetData(_disjoint, NULL, 0, 0) == S_FALSE);

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT tsDisjoint;
	pDeviceContext->GetData(_disjoint, &tsDisjoint, sizeof(tsDisjoint), 0);
	if (!(tsDisjoint.Disjoint))
	{
		UINT64 StartTime, StopTime;
		pDeviceContext->GetData(_start, &StartTime, sizeof(UINT64), 0);
		pDeviceContext->GetData(_stop, &StopTime, sizeof(UINT64), 0);
		double time = (double(StopTime - StartTime) / double(tsDisjoint.Frequency))*1000.0f;
		System::GetFileHandler()->DumpToFile("Recorded time for " + _name + " is: " + to_string(time));
		return time;
	}

	return -1;
}