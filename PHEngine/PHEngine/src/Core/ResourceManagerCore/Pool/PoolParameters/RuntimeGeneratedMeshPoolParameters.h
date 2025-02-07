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
    std::vector<std::shared_ptr<AttributeDataBase>> mVertexAttributes;

    RuntimeGeneratedMeshPoolParameters(const std::string& componentName, const size_t maxVerticesCount)
        : mComponentName(componentName)
        , mMaxVerticesCount(maxVerticesCount)
    {
    }

    friend struct std::hash<RuntimeGeneratedMeshPoolParameters>;

    bool operator==(const RuntimeGeneratedMeshPoolParameters& other) const
    {
        return this->mComponentName == other.mComponentName && this->mMaxVerticesCount == other.mMaxVerticesCount;
    }
};
} // namespace Resources

namespace std {
using namespace Resources;
template<>
struct hash<RuntimeGeneratedMeshPoolParameters> {
    std::size_t operator()(const RuntimeGeneratedMeshPoolParameters& params) const
    {
        return hash<std::string>()(params.mComponentName) ^ hash<size_t>()(params.mMaxVerticesCount);
    }
};
} // namespace std