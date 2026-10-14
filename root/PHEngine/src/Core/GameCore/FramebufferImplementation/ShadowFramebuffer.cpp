#include "ShadowFramebuffer.h"

namespace EngineCore {
namespace FramebufferImpl {

ShadowFramebuffer::ShadowFramebuffer(std::shared_ptr<ITexture> shadowMapTexture)
    : FramebufferBundle()
    , mShadowMapTexture(shadowMapTexture)
{
    Init();
}

ShadowFramebuffer::~ShadowFramebuffer()
{
}

void ShadowFramebuffer::SetTextures()
{
    mFramebuffer.AddRenderTexture(GL_DEPTH_ATTACHMENT, mShadowMapTexture);
}

void ShadowFramebuffer::SetFramebuffers()
{
    mFramebuffer.CreateFramebuffer();
}

void ShadowFramebuffer::SetRenderbuffers()
{
}

void ShadowFramebuffer::CleanUp()
{
    UnbindFramebuffer(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mFramebuffer.CleanUp();
}

void ShadowFramebuffer::RenderToTexture(
    bool bBindFramebuffer,
    const size_t viewportX,
    const size_t viewportY,
    const size_t viewportWidth,
    const size_t viewportHeight,
    const GLbitfield clearFlag)
{
    RenderToFBO(mFramebuffer, bBindFramebuffer, viewportX, viewportY, viewportWidth, viewportHeight, clearFlag);
}
} // namespace FramebufferImpl
} // namespace EngineCore
