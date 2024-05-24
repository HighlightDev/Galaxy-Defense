#pragma once

#include <string>
#include <cstdint>
#include <stdint.h>
#include <vector>

#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "Core/ResourceManagerCore/SimpleMeshType.h"

using namespace Graphics::OpenGL;

namespace Resources
{
    struct SimplePrimitivePoolParameters
    {
        SimplePrimitiveType mSimplePrimitiveType;
        std::vector<std::shared_ptr<AttributeDataBase>> mVertexAttributes;

        friend struct std::hash<SimplePrimitivePoolParameters>;

        bool operator==(const SimplePrimitivePoolParameters &other) const
        {
            return this->mSimplePrimitiveType == other.mSimplePrimitiveType;
        }
    };
}

namespace std
{
    using namespace Resources;
    template <>
    struct hash<SimplePrimitivePoolParameters>
    {
        std::size_t operator()(const SimplePrimitivePoolParameters &params) const
        {
            return hash<int32_t>()((int32_t)params.mSimplePrimitiveType);
        }
    };
}
