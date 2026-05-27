#pragma once

#include <memory>

namespace Graphics::Texture {
class ITexture;
} // namespace Graphics::Texture

namespace Graphics {
enum class ePostFxStageType { BLOOM_STAGE, GAUSSIAN_BLUR_STAGE };

class IPostFxRenderTargetProvider {
public:
    virtual ~IPostFxRenderTargetProvider() = default;

    virtual std::shared_ptr<::Graphics::Texture::ITexture>
    GetRenderTargetTextureByKey(const ePostFxStageType postFxStageType) const = 0;
};
} // namespace Graphics
