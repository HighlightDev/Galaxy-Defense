#pragma once

#include <string>
#include <cstdint>
#include <stdint.h>
#include <vector>

#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"

using namespace Graphics::OpenGL;

namespace Resources
{
    struct ParticlePoolParameters
    {
        std::string mParticleComponentName;
        size_t mParticleCount;
        std::vector<std::shared_ptr<AttributeDataBase>> mVertexAttributes;

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
    using namespace Resources;
    template <>
    struct hash<ParticlePoolParameters>
    {
        std::size_t operator()(const ParticlePoolParameters &params) const
        {
            return hash<std::string>()(params.mParticleComponentName) ^ hash<size_t>()(params.mParticleCount);
        }
    };
}
