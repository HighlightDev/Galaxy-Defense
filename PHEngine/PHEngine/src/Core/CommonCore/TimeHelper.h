#pragma once
#include <chrono>

using Clock_t = std::chrono::high_resolution_clock;
using Duration_t = Clock_t::duration;
using Moment_t = Clock_t::time_point;

namespace Game
{
    struct EngineTime
    {
        static Moment_t GetCurrentTime();

        static Duration_t GetPassedDuration(const Moment_t &lastTime);

        static double GetSecondsFromDuration(const Duration_t &time);
    };
}
