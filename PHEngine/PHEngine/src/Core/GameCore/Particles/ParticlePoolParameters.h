#pragma once

#include <string>
#include <cstdint>
#include <stdint.h>

namespace EngineCore
{
    struct ParticlePoolParameters
    {
        std::string mParticleComponentName;
        size_t mParticleCount;

        friend struct std::hash<ParticlePoolParameters>;

        bool operator==(const ParticlePoolParameters &other) const
        {
            return this->mParticleComponentName == other.mParticleComponentName &&
                   this->mParticleCount == other.mParticleCount;
        }
    };
}

namespace std
{
    using namespace EngineCore;
    template <>
    struct hash<ParticlePoolParameters>
    {
        std::size_t operator()(const ParticlePoolParameters &params) const
        {
            return hash<std::string>()(params.mParticleComponentName) ^ hash<size_t>()(params.mParticleCount);
        }
    };
}