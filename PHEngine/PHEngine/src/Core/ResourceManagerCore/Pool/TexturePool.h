#pragma once

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/ResourceManagerCore/Policy/TextureAllocationPolicy.h"
#include "Core/ResourceManagerCore/Pool/PoolBase.h"
#include "ITextureObtainable.h"

#include <memory>
#include <string>

using namespace Graphics::Texture;

namespace Resources {
class TexturePool : public PoolBase<ITexture, std::string, TextureAllocationPolicy>, public ITextureObtainable {
    static std::shared_ptr<TexturePool> m_instance;

public:
    using poolType_t = PoolBase<ITexture, std::string, TextureAllocationPolicy>;

    std::string ToString() const override;

    std::shared_ptr<ITexture> GetTextureAt(const size_t index) const override;

    size_t GetTexturesCount() const override;

    static std::shared_ptr<TexturePool> GetInstance();

    static void ReloadInstance();
};

} // namespace Resources