#pragma once

#include "Core/GameCore/GUI/Common/FontParams.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontAtlas.h"
#include "Core/ResourceManagerCore/Policy/FontMeshAllocationPolicy.h"
#include "Core/ResourceManagerCore/Pool/PoolBase.h"

using namespace Graphics::Mesh;
using namespace EngineCore;
using namespace EngineCore::GUI;

namespace Resources {
// class FreeTypeFontMeshPool : public PoolBase<FreeTypeFontAtlas, FontParams, FontMeshAllocationPolicy> {
//     static std::unique_ptr<FreeTypeFontMeshPool> m_instance;

// public:
//     using poolType_t = PoolBase<FreeTypeFontAtlas, FontParams, FontMeshAllocationPolicy>;

//     std::string ToString() const override;

//     static std::unique_ptr<FreeTypeFontMeshPool>& GetInstance();

//     static void ReloadInstance();
// };

} // namespace Resources
