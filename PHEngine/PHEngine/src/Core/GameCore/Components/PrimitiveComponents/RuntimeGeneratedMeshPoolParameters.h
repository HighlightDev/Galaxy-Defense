#pragma once

#include <string>
#include <cstdint>
#include <stdint.h>

namespace EngineCore
{
    struct RuntimeGeneratedMeshPoolParameters
    {
        std::string mComponentName;
        size_t mMaxVerticesCount;

        RuntimeGeneratedMeshPoolParameters(const std::string &componentName, const size_t maxVerticesCount)
            : mComponentName(componentName),
              mMaxVerticesCount(maxVerticesCount)
        {
        }

        friend struct std::hash<RuntimeGeneratedMeshPoolParameters>;

        bool operator==(const RuntimeGeneratedMeshPoolParameters &other) const
        {
            return this->mComponentName == other.mComponentName &&
                   this->mMaxVerticesCount == other.mMaxVerticesCount;
        }
    };
}

namespace std
{
    using namespace EngineCore;
    template <>
    struct hash<RuntimeGeneratedMeshPoolParameters>
    {
        std::size_t operator()(const RuntimeGeneratedMeshPoolParameters &params) const
        {
            return hash<std::string>()(params.mComponentName) ^ hash<size_t>()(params.mMaxVerticesCount);
        }
    };
}