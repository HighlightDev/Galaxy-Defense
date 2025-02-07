#pragma once

#include "Core/GraphicsCore/Mesh/AnimatedMeshData.h"
#include "Core/GraphicsCore/Mesh/MeshAttributes.h"

using namespace EngineCore;
using namespace Graphics::Mesh;

namespace IO {
struct MeshResourceInfo {
    AnimatedMeshData* meshAnimatedData = nullptr;
    MeshAttributes* meshAttributes = nullptr;

    ~MeshResourceInfo()
    {
        delete meshAttributes;
    }
};
} // namespace IO