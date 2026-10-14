#pragma once

#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferBundle.h"

using namespace Graphics;

namespace EngineCore {

namespace FramebufferImpl {
class PlanarReflectionFramebuffer : public FramebufferBundle {
    ViewPortInfo mReflectionViewPortInfo;

    std::shared_ptr<ITexture> mReflectionSurfaceTarget;
    std::shared_ptr<ITexture> mReflectionTexture;

    FramebufferObject mReflectionRenderTargetSurface;
    FramebufferObject mReflectionTextureSurface;

public:
    PlanarReflectionFramebuffer(const ViewPortInfo& reflectionViewPortInfo);

    ~PlanarReflectionFramebuffer() override;

    void RenderToTexture();

    void ResolveReflectionRenderTargetSurfaceData();

    std::shared_ptr<ITexture> GetReflectionTexture() const;

    void CleanUp() override;

protected:
    void SetTextures() override;

    void SetFramebuffers() override;

    void SetRenderbuffers() override;
};
} // namespace FramebufferImpl
} // namespace EngineCore
