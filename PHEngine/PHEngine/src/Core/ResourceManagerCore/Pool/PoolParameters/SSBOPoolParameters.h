#pragma once

#include <stdint.h>

#include <cstdint>
#include <functional>

namespace Resources {

struct SSBOPoolParameters {
    uint32_t bytesToAllocate;
    uint32_t bindingPoint;
    uint32_t flags;

    bool operator==(const SSBOPoolParameters& other) const
    {
        return this->bytesToAllocate == other.bytesToAllocate && this->bindingPoint == other.bindingPoint
            && this->flags == other.flags;
    }
};

} // namespace Resources

namespace std {
using namespace Resources;
template<>
struct hash<SSBOPoolParameters> {
    std::size_t operator()(const SSBOPoolParameters& params) const
    {
        return (
            hash<uint32_t>()(params.bytesToAllocate)
            ^ (hash<uint32_t>()(params.bindingPoint) ^ (hash<uint32_t>()(params.flags))));
    }
};
} // namespace std
