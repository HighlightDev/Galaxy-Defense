#include "ShadowFramebuffer.h"

namespace Game
{
   namespace FramebufferImpl
   {

      ShadowFramebuffer::ShadowFramebuffer(sharedTexture_t shadowmapTextureResource)
         : Framebuffer()
         , m_shadowmapTextureResource(shadowmapTextureResource)
      {
         Init();
      }

      ShadowFramebuffer::~ShadowFramebuffer()
      {
      }

      void ShadowFramebuffer::SetTextures()
      {
      }

      void ShadowFramebuffer::SetFramebuffers()
      {
         Base::GenFramebuffers(1);
         Base::BindFramebuffer(1);

         const auto textureType = m_shadowmapTextureResource->GetTextureType();

         if (TextureType::TEXTURE_CUBE == textureType)
         {
            Base::AttachCubeTextureToFramebuffer(GL_DEPTH_ATTACHMENT, m_shadowmapTextureResource);
         }
         else if (TextureType::TEXTURE_2D == textureType)
         {
            Base::Attach2DTextureToFramebuffer(GL_DEPTH_ATTACHMENT, m_shadowmapTextureResource);
         }

         glDrawBuffer(GL_NONE);
         glReadBuffer(GL_NONE);
      }

      void ShadowFramebuffer::SetRenderbuffers()
      {
      }

      void ShadowFramebuffer::CleanUp()
      {
         Base::CleanUp();
      }

   }
}
