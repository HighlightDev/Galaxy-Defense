#include "Random.h"

std::random_device Random::s_RandomDevice;
std::uniform_int_distribution<typename std::mt19937::result_type> Random::s_Distribution{0, std::numeric_limits<uint32_t>::max()};