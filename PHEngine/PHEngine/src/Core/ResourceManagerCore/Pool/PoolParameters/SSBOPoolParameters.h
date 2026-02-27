#pragma once

#include <stdint.h>

#include <cstdint>
#include <functional>

namespace Resources {

struct SSBOPoolParameters {
    uint32_t bytesToAllocate;
    uint32_t bindingPoint;
    uint32_t flags;
    bool bUniqueResource;

private:
    static uint32_t unique_index;
    uint32_t uniqueIndex;

public:
    explicit SSBOPoolParameters(uint32_t bytesToAllocate, uint32_t bindingPoint, uint32_t flags, bool bUniqueResource = true);

    bool operator==(const SSBOPoolParameters& other) const;
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
