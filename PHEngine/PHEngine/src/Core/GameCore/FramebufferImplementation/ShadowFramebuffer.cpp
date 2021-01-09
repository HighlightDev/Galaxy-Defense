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
         GenFramebuffers(1);
         BindFramebuffer(1);

         const auto textureType = m_shadowmapTextureResource->GetTextureType();

         if (TextureType::TEXTURE_CUBE == textureType)
         {
            AttachCubeTextureToFramebuffer(GL_DEPTH_ATTACHMENT, m_shadowmapTextureResource);
         }
         else if (TextureType::TEXTURE_2D == textureType)
         {
            Attach2DTextureToFramebuffer(GL_DEPTH_ATTACHMENT, m_shadowmapTextureResource);
         }

         glDrawBuffer(GL_NONE);
         glReadBuffer(GL_NONE);
      }

      void ShadowFramebuffer::BindTextureToRenderAttachment()
      {
         mBindings.AddBinding(1, GL_DEPTH_ATTACHMENT, m_shadowmapTextureResource);
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
