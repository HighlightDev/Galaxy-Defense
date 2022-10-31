#include "ResolvedSceneFramebuffer.h"

namespace Graphics
{
    // Buffer should be recreated when window size was changed
    ResolvedSceneFramebuffer::ResolvedSceneFramebuffer(const ViewPortInfo &viewPortInfo)
        : FramebufferBundle(),
          mViewPortInfo(viewPortInfo),
          m_resolvedSceneColorBuffer(),
          mFramebuffer(std::make_shared<FramebufferObject>())
    {
        Init();
    }

    ResolvedSceneFramebuffer::~ResolvedSceneFramebuffer()
    {
        DestroyResolvedSceneFramebuffer();
    }

    void ResolvedSceneFramebuffer::SetTextures()
    {
        TexParams sceneColorParams(mViewPortInfo.Width,
                                   mViewPortInfo.Height,
                                   GL_TEXTURE_2D,
                                   GL_NEAREST,
                                   GL_NEAREST,
                                   0,
                                   GL_RGB,
                                   GL_RGB,
                                   GL_UNSIGNED_BYTE,
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
        mFramebuffer->CreateRenderBuffer(GL_DEPTH_COMPONENT24, GL_DEPTH_ATTACHMENT, m_resolvedSceneColorBuffer->GetTextureRezolution());
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

    void ResolvedSceneFramebuffer::UnbindResolvedSceneFramebuffer()
    {
        UnbindFramebuffer();
    }

    void ResolvedSceneFramebuffer::BindResolvedSceneColorTexture(int32_t slot)
    {
        m_resolvedSceneColorBuffer->BindTexture(slot);
    }

    std::shared_ptr<ITexture> ResolvedSceneFramebuffer::GetResolvedSceneColorTexture() const
    {
        return m_resolvedSceneColorBuffer;
    }

    void ResolvedSceneFramebuffer::CopyFramebufferDataToDefaultFramebuffer(const size_t srcX, const size_t srcY, const size_t srcResolutionX, const size_t srcResolutionY,
                                                                           const size_t dstX, const size_t dstY, const size_t dstResolutionX, const size_t dstResolutionY, const int32_t bufferBit)
    {
        mFramebuffer->BindFramebuffer(GL_READ_FRAMEBUFFER, true, false);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(srcX, srcY, srcResolutionX, srcResolutionY, dstX, dstY, dstResolutionX, dstResolutionY, bufferBit, GL_NEAREST);
    }

    std::shared_ptr<IFramebufferObject> ResolvedSceneFramebuffer::GetFramebufferObjectInstance() const
    {
        return mFramebuffer;
    }
}