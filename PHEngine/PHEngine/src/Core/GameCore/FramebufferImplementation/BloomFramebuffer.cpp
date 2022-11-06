#include "BloomFramebuffer.h"
#include "Core/GraphicsCore/PostFX/Bloom/BloomConstants.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

using namespace Graphics;
using namespace EngineUtility;

namespace EngineCore
{
   namespace FramebufferImpl
   {
      // Buffer should be recreated when window size was changed
      BloomFramebuffer::BloomFramebuffer(const ViewPortInfo &viewPortInfo)
          : FramebufferBundle(),
            mViewPortInfo(viewPortInfo),
            mColor1Framebuffer(std::make_shared<FramebufferObject>()),
            mColor2Framebuffer(std::make_shared<FramebufferObject>())
      {
         const auto &cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();
         assert(BloomQualitySettings::s_blurQualityMap.count(cfg.BloomQualityName));
         const auto &bloomQuality = BloomQualitySettings::s_blurQualityMap.at(cfg.BloomQualityName);
         // scale bloom render target resolution accordingly to config file
         const auto bloomResolutionMultiplier = BloomQualitySettings::s_blurQualityMap.at(cfg.BloomQualityName).bloomResolutionMultiplier;
         mViewPortInfo = ViewPortInfo(mViewPortInfo.OriginX, mViewPortInfo.OriginY, static_cast<int32_t>(static_cast<float>(mViewPortInfo.Width) * bloomResolutionMultiplier),
                                      static_cast<int32_t>(static_cast<float>(mViewPortInfo.Height) * bloomResolutionMultiplier));
         Init();
      }

      BloomFramebuffer::~BloomFramebuffer()
      {
         if (m_color1 || m_color2)
         {
            DestroyBloomFramebuffer();
         }
      }

      void BloomFramebuffer::SetTextures()
      {

         // Color1 texture
         {
            TexParams color1Params(mViewPortInfo.Width,
                                   mViewPortInfo.Height,
                                   GL_TEXTURE_2D,
                                   GL_LINEAR,
                                   GL_LINEAR,
                                   0,
                                   GL_RGB,
                                   GL_RGB,
                                   GL_UNSIGNED_BYTE,
                                   GL_CLAMP_TO_EDGE,
                                   true);
            m_color1 = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(color1Params);
         }

         // Color2 texture
         {
            TexParams color2Params(mViewPortInfo.Width,
                                   mViewPortInfo.Height,
                                   GL_TEXTURE_2D,
                                   GL_LINEAR,
                                   GL_LINEAR,
                                   0,
                                   GL_RGB,
                                   GL_RGB,
                                   GL_UNSIGNED_BYTE,
                                   GL_CLAMP_TO_EDGE,
                                   true);
            m_color2 = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(color2Params);
         }

         mColor1Framebuffer->AddRenderTexture(GL_COLOR_ATTACHMENT0, m_color1);
         mColor2Framebuffer->AddRenderTexture(GL_COLOR_ATTACHMENT0, m_color2);
      }

      void BloomFramebuffer::SetFramebuffers()
      {
         mColor1Framebuffer->CreateFramebuffer();
         mColor2Framebuffer->CreateFramebuffer();
      }

      void BloomFramebuffer::SetRenderbuffers()
      {
      }

      void BloomFramebuffer::CleanUp()
      {
         DestroyBloomFramebuffer();
      }

      void BloomFramebuffer::DestroyBloomFramebuffer()
      {
         UnbindFramebuffer(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
         mColor1Framebuffer->CleanUp();
         mColor2Framebuffer->CleanUp();
         RenderTargetPool::GetInstance()->TryToFreeMemory(m_color1);
         RenderTargetPool::GetInstance()->TryToFreeMemory(m_color2);
         m_color1 = m_color2 = nullptr;
      }

      void BloomFramebuffer::BindColor1Framebuffer()
      {
         RenderToFBO(*mColor1Framebuffer, true, mViewPortInfo, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      }

      void BloomFramebuffer::BindColor2Framebuffer()
      {
         RenderToFBO(*mColor2Framebuffer, true, mViewPortInfo, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
   }
}