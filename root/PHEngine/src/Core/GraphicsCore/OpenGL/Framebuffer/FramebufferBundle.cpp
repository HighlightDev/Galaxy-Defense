#include "FramebufferBundle.h"

#include "Core/CommonCore/Assertion.h"

namespace Graphics {

FramebufferBundle::FramebufferBundle()
{
}

FramebufferBundle::~FramebufferBundle()
{
}

void FramebufferBundle::UnbindFramebuffer(const GLbitfield clearFlag) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    if (0 != clearFlag) {
        glClear(clearFlag);
    }
}

void FramebufferBundle::FramebufferClear(const FramebufferObject& fbo, const GLbitfield clearFlag)
{
    ext_assert(clearFlag, "FramebufferBundle::FramebufferClear: clearFlag is zero");
    fbo.BindFramebuffer(GL_FRAMEBUFFER, true, true);
    glClear(clearFlag);
}

void FramebufferBundle::Init()
{
    SetTextures();
    SetFramebuffers();
    SetRenderbuffers();
}

void FramebufferBundle::SetupViewport(
    size_t viewportX, size_t viewportY, size_t viewportWidth, size_t viewportHeight, GLbitfield clearFlag) const
{
    if (0 != clearFlag) {
        glClear(clearFlag);
    }
    glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
}

void FramebufferBundle::RenderToFBO(
    const FramebufferObject& fbo,
    bool bBindFramebuffer,
    size_t viewportX,
    size_t viewportY,
    size_t viewportWidth,
    size_t viewportHeight,
    GLbitfield clearFlag) const
{
    fbo.BindFramebuffer(GL_FRAMEBUFFER, bBindFramebuffer, true);
    SetupViewport(viewportX, viewportY, viewportWidth, viewportHeight, clearFlag);
}

void FramebufferBundle::RenderToFBO(
    const FramebufferObject& fbo, bool bBindFramebuffer, const glm::ivec4& viewport, GLbitfield clearFlag) const
{
    fbo.BindFramebuffer(GL_FRAMEBUFFER, bBindFramebuffer, true);
    SetupViewport(viewport.x, viewport.y, viewport.z, viewport.w, clearFlag);
}

void FramebufferBundle::RenderToFBO(
    const FramebufferObject& fbo, bool bBindFramebuffer, const ViewPortInfo& viewport, GLbitfield clearFlag) const
{
    fbo.BindFramebuffer(GL_FRAMEBUFFER, bBindFramebuffer, true);
    SetupViewport(viewport.OriginX, viewport.OriginY, viewport.Width, viewport.Height, clearFlag);
}

void FramebufferBundle::CopyFramebufferDataToDefaultFramebuffer(
    const size_t srcX,
    const size_t srcY,
    const size_t srcResolutionX,
    const size_t srcResolutionY,
    const size_t dstX,
    const size_t dstY,
    const size_t dstResolutionX,
    const size_t dstResolutionY,
    const int32_t bufferBit)
{
}

void FramebufferBundle::CopyFramebufferDataToDstFramebuffer(
    const std::shared_ptr<IFramebufferObject>& dstFramebuffer,
    const size_t srcX,
    const size_t srcY,
    const size_t srcResolutionX,
    const size_t srcResolutionY,
    const size_t dstX,
    const size_t dstY,
    const size_t dstResolutionX,
    const size_t dstResolutionY,
    const int32_t bufferBit)
{
}

void FramebufferBundle::CopySrcFramebufferDataToDefaultFramebufferImpl(
    const std::shared_ptr<IFramebufferObject>& srcFbo,
    const size_t srcX,
    const size_t srcY,
    const size_t srcResolutionX,
    const size_t srcResolutionY,
    const size_t dstX,
    const size_t dstY,
    const size_t dstResolutionX,
    const size_t dstResolutionY,
    const int32_t bufferBit)
{
    srcFbo->BindFramebufferAsReadTarget();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(
        srcX, srcY, srcResolutionX, srcResolutionY, dstX, dstY, dstResolutionX, dstResolutionY, bufferBit, GL_NEAREST);
}

void FramebufferBundle::CopySrcFramebufferDataToDstFramebufferImpl(
    const std::shared_ptr<IFramebufferObject>& srcFbo,
    const std::shared_ptr<IFramebufferObject>& dstFbo,
    const size_t srcX,
    const size_t srcY,
    const size_t srcResolutionX,
    const size_t srcResolutionY,
    const size_t dstX,
    const size_t dstY,
    const size_t dstResolutionX,
    const size_t dstResolutionY,
    const int32_t bufferBit)
{
    srcFbo->BindFramebufferAsReadTarget();
    dstFbo->BindFramebufferAsDrawTarget();
    glBlitFramebuffer(
        srcX, srcY, srcResolutionX, srcResolutionY, dstX, dstY, dstResolutionX, dstResolutionY, bufferBit, GL_NEAREST);
}
} // namespace Graphics
