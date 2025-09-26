#pragma once

#include "Core/GameCore/LoggerExtension.h"
#include "Core/GraphicsCore/Texture/CubemapTexture.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/GraphicsCore/Texture/Texture2d.h"

#include <memory>
#include <type_traits>

using namespace Graphics::Texture;
using namespace EngineCore;

namespace Resources {

class RenderTargetAllocationPolicy {
public:
    template<typename eTextureType = Texture2d>
    static inline std::enable_if_t<
        std::is_same_v<eTextureType, Texture2d> || std::is_same_v<eTextureType, CubemapTexture>,
        std::shared_ptr<ITexture>>
    AllocateMemory(const TexParams& texParams)
    {
        constexpr const char* textureTypeStr = std::is_same_v<eTextureType, Texture2d> ? "Texture2d" : "CubemapTexture";
        const std::string textureName = "render_target_" + std::string(textureTypeStr) + TexParams::ToString(texParams);
        LogInfo("RenderTargetAllocationPolicy::AllocateMemory: ", textureName);
        return std::make_shared<eTextureType>(texParams, textureName);
    }

    static inline std::shared_ptr<ITexture> AllocateMemory(const TexParams& texParams)
    {
        // to satisfy Allocatable concept
        assert(false);
        return nullptr;
    }

    static inline void DeallocateMemory(std::shared_ptr<ITexture> arg)
    {
        LogInfo("RenderTargetAllocationPolicy::DeallocateMemory: ", arg->GetTextureName());
        arg->CleanUp();
    }
};

} // namespace Resources
