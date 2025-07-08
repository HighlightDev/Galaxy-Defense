#pragma once

#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFont.h"
#include "Core/ResourceManagerCore/Policy/FreeTypeFontAllocationPolicy.h"
#include "Core/ResourceManagerCore/Pool/PoolBase.h"

#include <string>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace Resources {
class FreeTypeFontPool : public PoolBase<FreeTypeFont, std::string, FreeTypeFontAllocationPolicy> {
    static std::unique_ptr<FreeTypeFontPool> m_instance;

public:
    using poolType_t = PoolBase<FreeTypeFont, std::string, FreeTypeFontAllocationPolicy>;

    std::string ToString() const override;

    static std::unique_ptr<FreeTypeFontPool>& GetInstance();

    static void ReloadInstance();
};

} // namespace Resources
