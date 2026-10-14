#pragma once

#include "Core/GraphicsCore/Texture/ITexture.h"

#include <memory>

using namespace Graphics::Texture;

namespace Resources {
class ITextureObtainable {
public:
    virtual std::shared_ptr<ITexture> GetTextureAt(const size_t index) const = 0;

    virtual size_t GetTexturesCount() const = 0;

    virtual std::string GetTextureName(const std::shared_ptr<ITexture>& texture) const = 0;
};
} // namespace Resources