#include "ResolvedSceneFramebuffer.h"

#include "Core/UtilityCore/EngineConfigHolder.h"

using namespace EngineUtility;

namespace Graphics {
// Buffer should be recreated when window size was changed
ResolvedSceneFramebuffer::ResolvedSceneFramebuffer(const ViewPortInfo& viewPortInfo)
    : FramebufferBundle()
    , mViewPortInfo(viewPortInfo)
    , m_resolvedSceneColorBuffer()
    , mFramebuffer(std::make_shared<FramebufferObject>())
{
    Init();
}

ResolvedSceneFramebuffer::~ResolvedSceneFramebuffer()
{
}

void ResolvedSceneFramebuffer::SetTextures()
{
    const auto& cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();
    const bool isHdrEnabled = cfg.IsHdrEnabled;
    TexParams sceneColorParams(
        mViewPortInfo.Width,
        mViewPortInfo.Height,
        GL_TEXTURE_2D,
        GL_NEAREST,
        GL_NEAREST,
        0,
        isHdrEnabled ? GL_RGB16F : GL_RGB8,
        GL_RGB,
        isHdrEnabled ? GL_FLOAT : GL_UNSIGNED_BYTE,
        GL_REPEAT,
        true);
    m_resolvedSceneColorBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(sceneColorParams);

    mFramebuffer->AddRenderTexture(GL_COLOR_ATTACHMENT0, m_resolvedSceneColorBuffer);
}

void ResolvedSceneFramebuffer::SetFramebuffers()
{
    mFramebuffer->CreateFramebuffer();
}

void ResolvedSceneFramebuffer::SetRenderbuffers()
{
    mFramebuffer->BindFramebuffer(GL_FRAMEBUFFER, true);
    mFramebuffer->CreateRenderBuffer(
        GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_resolvedSceneColorBuffer->GetTextureResolution());
}

void ResolvedSceneFramebuffer::CleanUp()
{
    DestroyResolvedSceneFramebuffer();
}

void ResolvedSceneFramebuffer::DestroyResolvedSceneFramebuffer()
{
    mFramebuffer->UnbindFramebuffer();
    mFramebuffer->CleanUp();
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_resolvedSceneColorBuffer);
}

void ResolvedSceneFramebuffer::BindResolvedSceneFramebuffer(const int32_t clearBufferBit)
{
    RenderToFBO(*mFramebuffer, true, mViewPortInfo, clearBufferBit);
}

void ResolvedSceneFramebuffer::BindResolvedSceneColorTexture(int32_t slot)
{
    m_resolvedSceneColorBuffer->BindTexture(slot);
}

std::shared_ptr<ITexture> ResolvedSceneFramebuffer::GetResolvedSceneColorTexture() const
{
    return m_resolvedSceneColorBuffer;
}

void ResolvedSceneFramebuffer::CopyFramebufferDataToDefaultFramebuffer(
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
    FramebufferBundle::CopySrcFramebufferDataToDefaultFramebufferImpl(
        mFramebuffer, srcX, srcY, srcResolutionX, srcResolutionY, dstX, dstY, dstResolutionX, dstResolutionY, bufferBit);
}

void ResolvedSceneFramebuffer::CopyFramebufferDataToDstFramebuffer(
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
    FramebufferBundle::CopySrcFramebufferDataToDstFramebufferImpl(
        mFramebuffer,
        dstFramebuffer,
        srcX,
        srcY,
        srcResolutionX,
        srcResolutionY,
        dstX,
        dstY,
        dstResolutionX,
        dstResolutionY,
        bufferBit);
}

std::shared_ptr<IFramebufferObject> ResolvedSceneFramebuffer::GetFramebufferObjectInstance() const
{
    return mFramebuffer;
}

void ResolvedSceneFramebuffer::ResizeRenderTargets(const ViewPortInfo& viewPortInfo)
{
    mViewPortInfo = viewPortInfo;

    mFramebuffer->UnbindFramebuffer();
    TryToFreeRenderTargetTextures();

    AllocateTextures();
    mFramebuffer->ReassignRenderTexture(GL_COLOR_ATTACHMENT0, m_resolvedSceneColorBuffer);

    mFramebuffer->RebindFramebufferTextures();

    mFramebuffer->BindFramebuffer(GL_FRAMEBUFFER, true);
    mFramebuffer->ResizeRenderBufferStorage(GL_DEPTH24_STENCIL8, m_resolvedSceneColorBuffer->GetTextureResolution());
}

void ResolvedSceneFramebuffer::TryToFreeRenderTargetTextures()
{
    assert(m_resolvedSceneColorBuffer);
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_resolvedSceneColorBuffer);
}

void ResolvedSceneFramebuffer::AllocateTextures()
{
    const auto& cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();
    const bool isHdrEnabled = cfg.IsHdrEnabled;
    TexParams sceneColorParams(
        mViewPortInfo.Width,
        mViewPortInfo.Height,
        GL_TEXTURE_2D,
        GL_NEAREST,
        GL_NEAREST,
        0,
        isHdrEnabled ? GL_RGB16F : GL_RGB8,
        GL_RGB,
        isHdrEnabled ? GL_FLOAT : GL_UNSIGNED_BYTE,
        GL_REPEAT,
        true);
    m_resolvedSceneColorBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(sceneColorParams);
}
} // namespace Graphics