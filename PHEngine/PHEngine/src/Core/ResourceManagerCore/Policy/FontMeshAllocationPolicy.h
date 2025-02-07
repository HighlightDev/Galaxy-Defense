#pragma once
#include "Core/GameCore/GUI/Common/FontParams.h"
#include "Core/GameCore/GUI/Common/TextMesh.h"

#include <memory>

using namespace Graphics::Mesh;
using namespace EngineCore;

namespace Resources {
class FontMeshAllocationPolicy {
public:
    static std::shared_ptr<TextMesh> AllocateMemory(const FontParams& arg);

    static void DeallocateMemory(const std::shared_ptr<TextMesh>& arg);
};

} // namespace Resources
