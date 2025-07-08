#pragma once
#include "Core/GameCore/GUI/Common/FontParams.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFont.h"

#include <memory>

using namespace EngineCore::GUI;

namespace Resources {
class FreeTypeFontAllocationPolicy {
public:
    static std::shared_ptr<FreeTypeFont> AllocateMemory(const std::string& fontFile);

    static void DeallocateMemory(const std::shared_ptr<FreeTypeFont>& arg);
};

} // namespace Resources
