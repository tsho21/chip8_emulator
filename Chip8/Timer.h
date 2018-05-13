#pragma once
#ifndef _TIMER_H
#define _TIMER_H

#include <chrono>

#define NANO_SECONDS_PER_HZ 1000000000LL

typedef std::chrono::steady_clock Clock;

class Timer {

private:
    Clock::time_point _start_t;
    Clock::time_point _end_t;
    Clock::duration _elapsed;

public:
    void start();
    void end();
    long long elapsed();
};

#endif