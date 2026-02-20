#pragma once
#include <chrono>

using namespace std::chrono;
using Clock_t = std::chrono::high_resolution_clock;
using Duration_t = Clock_t::duration;
using Moment_t = std::chrono::time_point<std::chrono::system_clock>;

namespace EngineCore {
struct EngineTime {
    static Moment_t GetCurrentTime();

    static Duration_t GetPassedDuration(const Moment_t lastTime);

    static double GetTimeDifferenceInSeconds(const Duration_t duration);

    static double GetTimeDifferenceInMilliseconds(const Duration_t duration);

    static double GetTimeDifferenceInMicroseconds(const Duration_t duration);
};
} // namespace EngineCore
