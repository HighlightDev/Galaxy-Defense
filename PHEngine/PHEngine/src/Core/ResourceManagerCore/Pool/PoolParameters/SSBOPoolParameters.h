#pragma once

#include <stdint.h>

#include <cstdint>
#include <functional>

namespace Resources {

struct SSBOPoolParameters {
    uint32_t bytesToAllocate;
    uint32_t bindingPoint;

    bool operator==(const SSBOPoolParameters& other) const
    {
        return this->bytesToAllocate == other.bytesToAllocate && this->bindingPoint == other.bindingPoint;
    }
};

} // namespace Resources

namespace std {
using namespace Resources;
template<>
struct hash<SSBOPoolParameters> {
    std::size_t operator()(const SSBOPoolParameters& params) const
    {
        return (hash<uint32_t>()(params.bytesToAllocate) ^ (hash<uint32_t>()(params.bindingPoint)));
    }
};
} // namespace std
