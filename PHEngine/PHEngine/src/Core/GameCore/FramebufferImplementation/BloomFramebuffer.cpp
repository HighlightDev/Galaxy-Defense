#include "BloomFramebuffer.h"

#include "Core/GraphicsCore/PostFX/Bloom/BloomConstants.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

using namespace Graphics;
using namespace EngineUtility;

namespace EngineCore {
namespace FramebufferImpl {
// Buffer should be recreated when window size was changed
BloomFramebuffer::BloomFramebuffer(const ViewPortInfo& viewPortInfo)
    : FramebufferBundle()
    , mFullScreenResolutionViewPortInfo(viewPortInfo)
    , mColor1Framebuffer(std::make_shared<FramebufferObject>())
    , mColor2Framebuffer(std::make_shared<FramebufferObject>())
    , mResolvedBloomColorFramebuffer(std::make_shared<FramebufferObject>())
{
    const auto& cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();
    assert(BloomQualitySettings::s_blurQualityMap.count(cfg.BloomQualityName));
    const auto& bloomQuality = BloomQualitySettings::s_blurQualityMap.at(cfg.BloomQualityName);
    // scale bloom render target resolution accordingly to config file
    mQualityBloomResolutionMultiplier = BloomQualitySettings::s_blurQualityMap.at(cfg.BloomQualityName).bloomResolutionMultiplier;
    mIsHdrEnabled = cfg.IsHdrEnabled;
    mShrinkedResolutionViewPortInfo = ViewPortInfo(
        mFullScreenResolutionViewPortInfo.OriginX,
        mFullScreenResolutionViewPortInfo.OriginY,
        static_cast<int32_t>(static_cast<float>(mFullScreenResolutionViewPortInfo.Width) * mQualityBloomResolutionMultiplier),
        static_cast<int32_t>(static_cast<float>(mFullScreenResolutionViewPortInfo.Height) * mQualityBloomResolutionMultiplier));
    Init();
}

BloomFramebuffer::~BloomFramebuffer()
{
    if (m_color1 || m_color2) {
        DestroyBloomFramebuffer();
    }
}

void BloomFramebuffer::SetTextures()
{
    AllocateTextures();

    mColor1Framebuffer->AddRenderTexture(GL_COLOR_ATTACHMENT0, m_color1);
    mColor2Framebuffer->AddRenderTexture(GL_COLOR_ATTACHMENT0, m_color2);
    mResolvedBloomColorFramebuffer->AddRenderTexture(GL_COLOR_ATTACHMENT0, m_resolvedBloomColor);
}

void BloomFramebuffer::SetFramebuffers()
{
    mColor1Framebuffer->CreateFramebuffer();
    mColor2Framebuffer->CreateFramebuffer();
    mResolvedBloomColorFramebuffer->CreateFramebuffer();
}

void BloomFramebuffer::SetRenderbuffers()
{
    mColor1Framebuffer->BindFramebuffer(GL_FRAMEBUFFER, true);
    mColor1Framebuffer->CreateRenderBuffer(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_color1->GetTextureResolution());
    mResolvedBloomColorFramebuffer->BindFramebuffer(GL_FRAMEBUFFER, true);
    mResolvedBloomColorFramebuffer->CreateRenderBuffer(
        GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_resolvedBloomColor->GetTextureResolution());
}

void BloomFramebuffer::CleanUp()
{
    DestroyBloomFramebuffer();
}

void BloomFramebuffer::CleanColor1Framebuffer(const GLint clearBit)
{
    FramebufferClear(*mColor1Framebuffer, clearBit);
}

void BloomFramebuffer::CleanResolvedBloomColorFramebuffer(const GLint clearBit)
{
    FramebufferClear(*mResolvedBloomColorFramebuffer, clearBit);
}

void BloomFramebuffer::DestroyBloomFramebuffer()
{
    UnbindFramebuffer(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mColor1Framebuffer->CleanUp();
    mColor2Framebuffer->CleanUp();
    mResolvedBloomColorFramebuffer->CleanUp();
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_color1);
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_color2);
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_resolvedBloomColor);
    m_color1 = m_color2 = m_resolvedBloomColor = nullptr;
}

void BloomFramebuffer::BindColor1Framebuffer(const GLint clearBitFlag)
{
    RenderToFBO(*mColor1Framebuffer, true, mShrinkedResolutionViewPortInfo, clearBitFlag);
}

void BloomFramebuffer::BindColor2Framebuffer()
{
    RenderToFBO(
        *mColor2Framebuffer,
        true,
        mShrinkedResolutionViewPortInfo,
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void BloomFramebuffer::BindResolvedBloomColorFramebuffer()
{
    // don't clear stencil, because we need it to get rid of bloom effect on neighbour pixels
    RenderToFBO(
        *mResolvedBloomColorFramebuffer, true, mFullScreenResolutionViewPortInfo, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BloomFramebuffer::BindColor1Texture(int32_t slot)
{
    m_color1->BindTexture(slot);
}

void BloomFramebuffer::BindColor2Texture(int32_t slot)
{
    m_color2->BindTexture(slot);
}

std::shared_ptr<ITexture> BloomFramebuffer::GetColor1Texture() const
{
    return m_color1;
}

std::shared_ptr<ITexture> BloomFramebuffer::GetColor2Texture() const
{
    return m_color2;
}

std::shared_ptr<ITexture> BloomFramebuffer::GetResolvedBloomTexture() const
{
    return m_resolvedBloomColor;
}

std::shared_ptr<IFramebufferObject> BloomFramebuffer::GetColor1FramebufferObjectInstance() const
{
    return mColor1Framebuffer;
}

std::shared_ptr<IFramebufferObject> BloomFramebuffer::GetResolvedBloomColorFramebufferObjectInstance() const
{
    return mResolvedBloomColorFramebuffer;
}

ViewPortInfo BloomFramebuffer::GetFullScreenResolutionViewPortInfo() const
{
    return mFullScreenResolutionViewPortInfo;
}

ViewPortInfo BloomFramebuffer::GetShrinkedResolutionViewPortInfo() const
{
    return mShrinkedResolutionViewPortInfo;
}

void BloomFramebuffer::ResizeRenderTargets(const ViewPortInfo& viewPortInfo)
{
    mFullScreenResolutionViewPortInfo = viewPortInfo;
    mShrinkedResolutionViewPortInfo = ViewPortInfo(
        mFullScreenResolutionViewPortInfo.OriginX,
        mFullScreenResolutionViewPortInfo.OriginY,
        static_cast<int32_t>(static_cast<float>(mFullScreenResolutionViewPortInfo.Width) * mQualityBloomResolutionMultiplier),
        static_cast<int32_t>(static_cast<float>(mFullScreenResolutionViewPortInfo.Height) * mQualityBloomResolutionMultiplier));

    mColor1Framebuffer->UnbindFramebuffer();
    TryToFreeRenderTargetTextures();
    AllocateTextures();

    mColor1Framebuffer->ReassignRenderTexture(GL_COLOR_ATTACHMENT0, m_color1);
    mColor2Framebuffer->ReassignRenderTexture(GL_COLOR_ATTACHMENT0, m_color2);
    mResolvedBloomColorFramebuffer->ReassignRenderTexture(GL_COLOR_ATTACHMENT0, m_resolvedBloomColor);

    mColor1Framebuffer->RebindFramebufferTextures();
    mColor2Framebuffer->RebindFramebufferTextures();
    mResolvedBloomColorFramebuffer->RebindFramebufferTextures();

    mColor1Framebuffer->BindFramebuffer(GL_FRAMEBUFFER, true);
    mColor1Framebuffer->ResizeRenderBufferStorage(GL_DEPTH24_STENCIL8, m_color1->GetTextureResolution());

    mResolvedBloomColorFramebuffer->BindFramebuffer(GL_FRAMEBUFFER, true);
    mResolvedBloomColorFramebuffer->ResizeRenderBufferStorage(GL_DEPTH24_STENCIL8, m_resolvedBloomColor->GetTextureResolution());
    mResolvedBloomColorFramebuffer->UnbindFramebuffer();
}

void BloomFramebuffer::AllocateTextures()
{
    // Color1 texture
    {
        TexParams color1Params(
            mShrinkedResolutionViewPortInfo.Width,
            mShrinkedResolutionViewPortInfo.Height,
            GL_TEXTURE_2D,
            GL_LINEAR,
            GL_LINEAR,
            0,
            mIsHdrEnabled ? GL_RGB16F : GL_RGB8,
            GL_RGB,
            mIsHdrEnabled ? GL_FLOAT : GL_UNSIGNED_BYTE,
            GL_CLAMP_TO_EDGE,
            true);
        m_color1 = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(color1Params);
    }

    // Color2 texture
    {
        TexParams color2Params(
            mShrinkedResolutionViewPortInfo.Width,
            mShrinkedResolutionViewPortInfo.Height,
            GL_TEXTURE_2D,
            GL_LINEAR,
            GL_LINEAR,
            0,
            mIsHdrEnabled ? GL_RGB16F : GL_RGB8,
            GL_RGB,
            mIsHdrEnabled ? GL_FLOAT : GL_UNSIGNED_BYTE,
            GL_CLAMP_TO_EDGE,
            true);
        m_color2 = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(color2Params);
    }

    // ResvoledBloomColor texture
    {
        TexParams resvoledBloomColorParams(
            mFullScreenResolutionViewPortInfo.Width,
            mFullScreenResolutionViewPortInfo.Height,
            GL_TEXTURE_2D,
            GL_LINEAR,
            GL_LINEAR,
            0,
            mIsHdrEnabled ? GL_RGB16F : GL_RGB8,
            GL_RGB,
            mIsHdrEnabled ? GL_FLOAT : GL_UNSIGNED_BYTE,
            GL_CLAMP_TO_EDGE,
            true);

        m_resolvedBloomColor = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(resvoledBloomColorParams);
    }
}

void BloomFramebuffer::TryToFreeRenderTargetTextures()
{
    assert(m_color1 && m_color2 && m_resolvedBloomColor);
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_color1);
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_color2);
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_resolvedBloomColor);
}
} // namespace FramebufferImpl
} // namespace EngineCore