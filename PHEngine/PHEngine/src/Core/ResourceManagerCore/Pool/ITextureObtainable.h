#pragma once

#include <memory>

#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Graphics::Texture;

namespace Resources
{
    class ITextureObtainable
    {
    public:
        virtual std::shared_ptr<ITexture> GetTextureAt(const size_t index) const = 0;

        virtual size_t GetTexturesCount() const = 0;
    };
}