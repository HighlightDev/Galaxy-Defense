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
        static constexpr double invFromNanoToSec = 0.000000001;
        return static_cast<double>(duration.count()) * invFromNanoToSec;
    }
}
