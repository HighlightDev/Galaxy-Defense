#pragma once

#include <stdint.h>

#include <cstdint>
#include <limits>
#include <random>

#undef max

class Random {
public:
    static float Float()
    {
        return (float)s_Distribution(s_RandomDevice) / (float)std::numeric_limits<uint32_t>::max();
    }

private:
    static std::random_device s_RandomDevice;
    static std::uniform_int_distribution<typename std::mt19937::result_type> s_Distribution;
};