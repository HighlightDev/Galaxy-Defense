#pragma once

#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"

#include <stdint.h>

#include <cstdint>
#include <string>
#include <vector>

using namespace Graphics::OpenGL;

namespace Resources {
struct ParticlePoolParameters {
    std::string mParticleComponentName;
    size_t mParticleCount;
    std::vector<std::shared_ptr<AttributeDataBase>> mVertexAttributes;

    friend struct std::hash<ParticlePoolParameters>;

    bool operator==(const ParticlePoolParameters& other) const
    {
        return this->mParticleComponentName == other.mParticleComponentName && this->mParticleCount == other.mParticleCount;
    }
};
} // namespace Resources

namespace std {
using namespace Resources;
template<>
struct hash<ParticlePoolParameters> {
    std::size_t operator()(const ParticlePoolParameters& params) const
    {
        return hash<std::string>()(params.mParticleComponentName) ^ hash<size_t>()(params.mParticleCount);
    }
};
} // namespace std
