#pragma once
#include <chrono>

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
