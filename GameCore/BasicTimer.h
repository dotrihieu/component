﻿#ifndef BASIC_TIMER_H
#define BASIC_TIMER_H
#ifdef USE_DIRECTX
#include <wrl.h>
#endif
#ifdef WIN32
#include <windows.h>
#endif

// Helper class for basic timing.
class BasicTimer
{
public:
	// Initializes internal timer values.
	BasicTimer();
	
	// Reset the timer to initial values.
	void Reset();
	
	// Update the timer's internal values.
	void Update();
	
	// Duration in seconds between the last call to Reset() and the last call to Update().
	float GetTotalAtFrameBegin();
	
	// Duration in seconds between the previous two calls to Update().
	float GetDeltaAtFrameBegin();

	float GetCurrentTime();

private:
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_currentTime;
	LARGE_INTEGER m_startTime;
	LARGE_INTEGER m_lastTime;
	float m_total;
	float m_delta;
};

#endif