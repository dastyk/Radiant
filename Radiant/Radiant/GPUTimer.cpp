#include "GPUTimer.h"
#include "System.h"


GPUTimer::GPUTimer()
{
	_start = nullptr;
	_stop = nullptr;
	_disjoint = nullptr;

	Graphics* g = System::GetGraphics();

	//HRESULT hr;
	//D3D11_QUERY_DESC desc;
	//desc.Query = D3D11_QUERY_TIMESTAMP;
	//desc.MiscFlags = 0;
	//hr = pDevice->CreateQuery(&desc, &mStart);
	//if (FAILED(hr))
	//{
	//	MessageBox(0, L"Could not create D3D11_QUERY_TIMESTAMP.", 0, 0);
	//	return false;
	//}

	//hr = pDevice->CreateQuery(&desc, &mStop);
	//if (FAILED(hr))
	//{
	//	MessageBox(0, L"Could not create D3D11_QUERY_TIMESTAMP.", 0, 0);
	//	return false;
	//}

	//desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	//hr = pDevice->CreateQuery(&desc, &mDisjoint);
	//if (FAILED(hr))
	//{
	//	MessageBox(0, L"Could not create D3D11_QUERY_TIMESTAMP_DISJOINT.", 0, 0);
	//	return false;
	//}
}


GPUTimer::~GPUTimer()
{
	SAFE_RELEASE(_start);
	SAFE_RELEASE(_stop);
	SAFE_RELEASE(_disjoint);
}

const void GPUTimer::TimeStart()
{

}

const void GPUTimer::TimeEnd()
{

}

const double GPUTimer::GetTime()const
{

}