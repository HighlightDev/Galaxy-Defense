#pragma once

#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFont.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontAtlas.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontParams.h"
#include "Core/ResourceManagerCore/Policy/FreeTypeFontMeshAllocationPolicy.h"
#include "Core/ResourceManagerCore/Pool/PoolBase.h"

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace Resources {
class FreeTypeFontMeshPool : public PoolBase<FreeTypeFontAtlas, FreeTypeFontParams, FreeTypeFontMeshAllocationPolicy> {
    static std::unique_ptr<FreeTypeFontMeshPool> m_instance;

public:
    using poolType_t = PoolBase<FreeTypeFontAtlas, FreeTypeFontParams, FreeTypeFontMeshAllocationPolicy>;

    FreeTypeFontMeshPool()
        : poolType_t([]() { FreeTypeFont::UnloadFreeTypeFontLibrary(); })
    {
    }

    std::string ToString() const override;

    static std::unique_ptr<FreeTypeFontMeshPool>& GetInstance();

    static void ReloadInstance();
};

} // namespace Resources
