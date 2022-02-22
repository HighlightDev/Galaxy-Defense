#include "ShadowFramebuffer.h"

namespace EngineCore
{
   namespace FramebufferImpl
   {

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
      }

      void ShadowFramebuffer::RenderToTexture(bool bBindFramebuffer, const size_t viewportX, const size_t viewportY, const size_t viewportWidth, const size_t viewportHeight, const GLbitfield clearFlag)
      {
         RenderToFBO(mFramebuffer, bBindFramebuffer, viewportX, viewportY, viewportWidth, viewportHeight, clearFlag);
      }
   }
}
