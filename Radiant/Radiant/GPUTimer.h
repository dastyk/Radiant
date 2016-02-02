#ifndef _GPUTIMER_H_
#define _GPUTIMER_H_


#pragma once
#include <d3d11.h>
#include "General.h"
#include "Utils.h"

class GPUTimer
{
public:
	GPUTimer(const std::string& name);
	~GPUTimer();

	const void TimeStart();
	const void TimeEnd();
	const double GetTime()const;

private:

	ID3D11Query* _start, *_stop, *_disjoint;
	std::string _name;
};

#endif