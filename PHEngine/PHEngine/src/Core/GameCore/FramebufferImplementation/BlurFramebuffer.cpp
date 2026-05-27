#include "BlurFramebuffer.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/PostFX/Bloom/BloomConstants.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

using namespace Graphics;
using namespace EngineUtility;

namespace EngineCore {
namespace FramebufferImpl {
// Buffer should be recreated when window size was changed
BlurFramebuffer::BlurFramebuffer(const ViewPortInfo& viewPortInfo)
    : FramebufferBundle()
    , mShrinkedResolutionViewPortInfo(viewPortInfo)
    , m_color1(nullptr)
    , m_color2(nullptr)
    , mColor1Framebuffer(std::make_shared<FramebufferObject>())
    , mColor2Framebuffer(std::make_shared<FramebufferObject>())
{
    const auto& cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();
    ext_assert(
        BloomQualitySettings::s_blurQualityMap.count(cfg.BloomQualityName),
        "Unknown bloom quality name: " + cfg.BloomQualityName);
    const auto& bloomQuality = BloomQualitySettings::s_blurQualityMap.at(cfg.BloomQualityName);
    // scale bloom render target resolution accordingly to config file
    mQualityBloomResolutionMultiplier = BloomQualitySettings::s_blurQualityMap.at(cfg.BloomQualityName).bloomResolutionMultiplier;
    mShrinkedResolutionViewPortInfo = ViewPortInfo(
        viewPortInfo.OriginX,
        viewPortInfo.OriginY,
        static_cast<int32_t>(static_cast<float>(viewPortInfo.Width) * mQualityBloomResolutionMultiplier),
        static_cast<int32_t>(static_cast<float>(viewPortInfo.Height) * mQualityBloomResolutionMultiplier));
    Init();
}

BlurFramebuffer::~BlurFramebuffer()
{
    if (m_color1 || m_color2) {
        DestroyBlurFramebuffer();
    }
}

void BlurFramebuffer::SetTextures()
{
    AllocateTextures();

    mColor1Framebuffer->AddRenderTexture(GL_COLOR_ATTACHMENT0, m_color1);
    mColor2Framebuffer->AddRenderTexture(GL_COLOR_ATTACHMENT0, m_color2);
}

void BlurFramebuffer::SetFramebuffers()
{
    mColor1Framebuffer->CreateFramebuffer();
    mColor2Framebuffer->CreateFramebuffer();
}

void BlurFramebuffer::SetRenderbuffers()
{
}

void BlurFramebuffer::CleanUp()
{
    DestroyBlurFramebuffer();
}

void BlurFramebuffer::CleanColor1Framebuffer(const GLint clearBit)
{
    FramebufferClear(*mColor1Framebuffer, clearBit);
}

void BlurFramebuffer::DestroyBlurFramebuffer()
{
    UnbindFramebuffer(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mColor1Framebuffer->CleanUp();
    mColor2Framebuffer->CleanUp();
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_color1);
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_color2);
    m_color1 = m_color2 = nullptr;
}

void BlurFramebuffer::BindColor1Framebuffer(const GLint clearBitFlag)
{
    RenderToFBO(*mColor1Framebuffer, true, mShrinkedResolutionViewPortInfo, clearBitFlag);
}

void BlurFramebuffer::BindColor2Framebuffer(const GLint clearBitFlag)
{
    RenderToFBO(*mColor2Framebuffer, true, mShrinkedResolutionViewPortInfo, clearBitFlag);
}

void BlurFramebuffer::BindColor1Texture(int32_t slot)
{
    m_color1->BindTexture(slot);
}

void BlurFramebuffer::BindColor2Texture(int32_t slot)
{
    m_color2->BindTexture(slot);
}

std::shared_ptr<ITexture> BlurFramebuffer::GetColor1Texture() const
{
    return m_color1;
}

std::shared_ptr<ITexture> BlurFramebuffer::GetColor2Texture() const
{
    return m_color2;
}

std::shared_ptr<IFramebufferObject> BlurFramebuffer::GetColor1FramebufferObjectInstance() const
{
    return mColor1Framebuffer;
}

std::shared_ptr<IFramebufferObject> BlurFramebuffer::GetColor2FramebufferObjectInstance() const
{
    return mColor2Framebuffer;
}

ViewPortInfo BlurFramebuffer::GetShrinkedResolutionViewPortInfo() const
{
    return mShrinkedResolutionViewPortInfo;
}

void BlurFramebuffer::ResizeRenderTargets(const ViewPortInfo& viewPortInfo)
{
    mShrinkedResolutionViewPortInfo = ViewPortInfo(
        viewPortInfo.OriginX,
        viewPortInfo.OriginY,
        static_cast<int32_t>(static_cast<float>(viewPortInfo.Width) * mQualityBloomResolutionMultiplier),
        static_cast<int32_t>(static_cast<float>(viewPortInfo.Height) * mQualityBloomResolutionMultiplier));

    mColor1Framebuffer->UnbindFramebuffer();
    TryToFreeRenderTargetTextures();
    AllocateTextures();

    mColor1Framebuffer->ReassignRenderTexture(GL_COLOR_ATTACHMENT0, m_color1);
    mColor2Framebuffer->ReassignRenderTexture(GL_COLOR_ATTACHMENT0, m_color2);

    mColor1Framebuffer->RebindFramebufferTextures();
    mColor2Framebuffer->RebindFramebufferTextures();
}

void BlurFramebuffer::AllocateTextures()
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
            GL_RGB8,
            GL_RGB,
            GL_UNSIGNED_BYTE,
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
            GL_RGB8,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            GL_CLAMP_TO_EDGE,
            true);
        m_color2 = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(color2Params);
    }
}

void BlurFramebuffer::TryToFreeRenderTargetTextures()
{
    ext_assert(m_color1 && m_color2, "BlurFramebuffer::TryToFreeRenderTargetTextures: render target textures are null");
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_color1);
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_color2);
}
} // namespace FramebufferImpl
} // namespace EngineCore