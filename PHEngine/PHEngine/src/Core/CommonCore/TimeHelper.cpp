#include "TimeHelper.h"

namespace Game
{

    Moment_t EngineTime::GetCurrentTime()
    {
        return Clock_t::now();
    }

    Duration_t EngineTime::GetPassedDuration(const Moment_t &lastTime)
    {
        return GetCurrentTime() - lastTime;
    }

    double EngineTime::GetSecondsFromDuration(const Duration_t &duration)
    {
        static constexpr double invFromNanoToSec = 1e-9;
        return static_cast<double>(duration.count()) * invFromNanoToSec;
    }

    double EngineTime::GetMillisecondsFromDuration(const Duration_t &duration)
    {
        static constexpr double invFromNanoToMilliSec = 1e-6;
        return static_cast<double>(duration.count()) * invFromNanoToMilliSec;
    }

    double EngineTime::GetMicrosecondsFromDuration(const Duration_t &duration)
    {
        static constexpr double invFromNanoToMilliSec = 1e-3;
        return static_cast<double>(duration.count()) * invFromNanoToMilliSec;
    }
}
