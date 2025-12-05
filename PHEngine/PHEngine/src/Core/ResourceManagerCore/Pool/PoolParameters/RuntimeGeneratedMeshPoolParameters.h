#pragma once

#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"

#include <stdint.h>

#include <cstdint>
#include <string>
#include <vector>

using namespace Graphics::OpenGL;

namespace Resources {
struct RuntimeGeneratedMeshPoolParameters {
    std::string mComponentName;
    size_t mMaxVerticesCount;
    size_t mMaxIndicesCount;
    std::vector<std::shared_ptr<AttributeDataBase>> mVertexAttributes;

    RuntimeGeneratedMeshPoolParameters(
        const std::string& componentName, const size_t maxVerticesCount, const size_t maxIndicesCount)
        : mComponentName(componentName)
        , mMaxVerticesCount(maxVerticesCount)
        , mMaxIndicesCount(maxIndicesCount)
    {
    }

    RuntimeGeneratedMeshPoolParameters()
        : mComponentName()
        , mMaxVerticesCount(0)
        , mMaxIndicesCount(0)
    {
    }

    friend struct std::hash<RuntimeGeneratedMeshPoolParameters>;

    bool operator==(const RuntimeGeneratedMeshPoolParameters& other) const
    {
        return this->mComponentName == other.mComponentName && this->mMaxVerticesCount == other.mMaxVerticesCount
            && this->mMaxIndicesCount == other.mMaxIndicesCount;
    }
};
} // namespace Resources

namespace std {
using namespace Resources;
template<>
struct hash<RuntimeGeneratedMeshPoolParameters> {
    std::size_t operator()(const RuntimeGeneratedMeshPoolParameters& params) const
    {
        return hash<std::string>()(params.mComponentName) ^ hash<size_t>()(params.mMaxVerticesCount)
            ^ hash<size_t>()(params.mMaxIndicesCount);
    }
};
} // namespace std