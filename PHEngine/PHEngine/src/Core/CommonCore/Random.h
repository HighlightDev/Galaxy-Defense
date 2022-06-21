#pragma once

#include <random>
#include <limits>
#include <stdint.h>
#include <cstdint>

#undef max

class Random
{
public:
    static float Float()
    {
        return (float)s_Distribution(s_RandomDevice) / (float)std::numeric_limits<uint32_t>::max();
    }

    static uint32_t UInt32()
    {
        return s_Distribution(s_RandomDevice) / (float)std::numeric_limits<uint32_t>::max();
    }

private:
    // static std::mt19937 s_RandomEngine;
    static std::random_device s_RandomDevice;
    static std::uniform_int_distribution<typename std::mt19937::result_type> s_Distribution;
};