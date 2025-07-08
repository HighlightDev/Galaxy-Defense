#pragma once
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontAtlas.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontParams.h"

#include <memory>

using namespace EngineCore::GUI;

namespace Resources {
class FreeTypeFontMeshAllocationPolicy {
public:
    static std::shared_ptr<FreeTypeFontAtlas> AllocateMemory(const FreeTypeFontParams& arg);

    static void DeallocateMemory(const std::shared_ptr<FreeTypeFontAtlas>& arg);
};

} // namespace Resources
