#pragma once

#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"

#include <stdint.h>

#include <cstdint>
#include <string>
#include <vector>

using namespace Graphics::OpenGL;

namespace Resources {
struct MeshPoolParameters {
    std::string mModelPath;
    std::vector<std::shared_ptr<AttributeDataBase>> mVertexAttributes;

    friend struct std::hash<MeshPoolParameters>;

    bool operator==(const MeshPoolParameters& other) const
    {
        return this->mModelPath == other.mModelPath;
    }
};
} // namespace Resources

namespace std {
using namespace Resources;
template<>
struct hash<MeshPoolParameters> {
    std::size_t operator()(const MeshPoolParameters& params) const
    {
        return hash<std::string>()(params.mModelPath);
    }
};
} // namespace std
