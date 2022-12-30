#pragma once
#include <memory>

#include "Core/GameCore/GUI/Common/TextMesh.h"
#include "Core/GameCore/GUI/Common/FontParams.h"

using namespace Graphics::Mesh;
using namespace EngineCore;

namespace Resources
{
    class FontMeshAllocationPolicy
    {
    public:
        static std::shared_ptr<TextMesh> AllocateMemory(const FontParams &arg);

        static void DeallocateMemory(const std::shared_ptr<TextMesh> &arg);
    };

}
