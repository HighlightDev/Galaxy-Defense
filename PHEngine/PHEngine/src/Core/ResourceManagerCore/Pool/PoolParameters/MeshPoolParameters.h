#pragma once

#include <string>
#include <cstdint>
#include <stdint.h>
#include <vector>

#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"

using namespace Graphics::OpenGL;

namespace Resources
{
    struct MeshPoolParameters
    {
        std::string mModelPath;
        std::vector<std::shared_ptr<AttributeDataBase>> mVertexAttributes;

        friend struct std::hash<MeshPoolParameters>;

        bool operator==(const MeshPoolParameters &other) const
        {
            return this->mModelPath == other.mModelPath;
        }
    };
}

namespace std
{
    using namespace Resources;
    template <>
    struct hash<MeshPoolParameters>
    {
        std::size_t operator()(const MeshPoolParameters &params) const
        {
            return hash<std::string>()(params.mModelPath);
        }
    };
}
