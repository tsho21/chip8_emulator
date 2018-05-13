#include "Timer.h"
#include "assert.h"

void Timer::start() {
    _start_t = Clock::now();
}

void Timer::end() {
    _end_t = Clock::now();
}

long long Timer::elapsed() {
    Clock::duration elapsed = _end_t - _start_t;
    return elapsed.count();
}