#pragma once
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/ResourceManagerCore/Policy/RenderTargetAllocationPolicy.h"
#include "ITextureObtainable.h"
#include "PoolBase.h"

#include <iterator>

using namespace Graphics::Texture;

namespace Resources {
class RenderTargetPool : public PoolBase<ITexture, TexParams, RenderTargetAllocationPolicy>, public ITextureObtainable {

    static std::shared_ptr<RenderTargetPool> m_instance;

public:
    using poolType_t = PoolBase<ITexture, TexParams, RenderTargetAllocationPolicy>;

    std::string ToString() const override;

    std::shared_ptr<ITexture> GetTextureAt(const size_t index) const override;

    size_t GetTexturesCount() const override;

    static std::shared_ptr<RenderTargetPool> GetInstance();

    static void ReloadInstance();
};
} // namespace Resources
