#include "TimeHelper.h"

namespace EngineCore {

Moment_t EngineTime::GetCurrentTime()
{
    return std::chrono::system_clock::now();
}

Duration_t EngineTime::GetPassedDuration(const Moment_t lastTime)
{
    return GetCurrentTime() - lastTime;
}

double EngineTime::GetTimeDifferenceInSeconds(const Duration_t duration)
{
    static constexpr double invFromNanoToSec = 1e-9;
    return static_cast<double>(duration.count()) * invFromNanoToSec;
}

double EngineTime::GetTimeDifferenceInMilliseconds(const Duration_t duration)
{
    static constexpr double invFromNanoToMilliSec = 1e-6;
    return static_cast<double>(duration.count()) * invFromNanoToMilliSec;
}

double EngineTime::GetTimeDifferenceInMicroseconds(const Duration_t duration)
{
    static constexpr double invFromNanoToMicroSec = 1e-3;
    return static_cast<double>(duration.count()) * invFromNanoToMicroSec;
}
} // namespace EngineCore
