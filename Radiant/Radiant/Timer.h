#ifndef _TIMER_H_
#define _TIMER_H_

#pragma once
//////////////
// Includes //
//////////////
#include <Windows.h>

class Timer
{
public:
	Timer();
	~Timer();

	const float DeltaTime()const;
	const float TotalTime()const;

	const void Reset();
	const void Start();
	const void Stop();
	const void Tick();

private:
	double _secondsPerCount;
	double _deltaTime;
	__int64 _pausedTime;
	__int64 _baseTime;
	__int64 _stopTime;
	__int64 _prevTime;
	__int64 _currTime;

	bool _stopped;
};

#endif