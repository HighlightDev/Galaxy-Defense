#pragma once

#include "Core/GraphicsCore/PostFX/IPostFxPass.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

namespace EngineCore::ShaderImpl {
class BloomFxShader;
}

namespace EngineCore::FramebufferImpl {
class BlurFramebuffer;
}

namespace Graphics {
class FramebufferBundle;

namespace Texture {
class ITexture;
}

class GaussianBlurPostFxPass : public IPostFxPass {

    std::shared_ptr<EngineCore::ShaderImpl::BloomFxShader> mBlurShader;

    std::shared_ptr<EngineCore::FramebufferImpl::BlurFramebuffer> mBlurFramebuffer;

    int32_t mBlurPassCount;

public:
    explicit GaussianBlurPostFxPass(const ViewPortInfo& viewPortInfo);

    ~GaussianBlurPostFxPass() override;

    void ResizeRenderTargets(const ViewPortInfo& viewPortInfo) override;

    void ExecutePostFx(
        const std::shared_ptr<ITexture>& sceneColorTexture,
        const std::shared_ptr<FramebufferBundle>& previousStepFramebuffer) override;

    std::shared_ptr<ITexture> GetPostFxResult() const override;

    void CleanUp() override;

private:
    void Init();
};

} // namespace Graphics