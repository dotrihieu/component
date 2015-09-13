#ifndef TIMER_H
#define TIMER_H
#ifdef WIN32
#include "WinBase.h"
#endif
class Timer
{
#ifdef WIN32
	static long long milliseconds_now() {
    static LARGE_INTEGER s_frequency;
    static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
    if (s_use_qpc) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / s_frequency.QuadPart;
    } else {
        return GetTickCount();
    }
}
#endif
};
#endif