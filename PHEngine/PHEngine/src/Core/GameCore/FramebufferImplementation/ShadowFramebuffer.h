#pragma once
#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferBundle.h"
#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferObject.h"

using namespace Graphics;

namespace EngineCore
{
   namespace FramebufferImpl
   {

      class ShadowFramebuffer :
         public FramebufferBundle
      {
         using Base = FramebufferBundle;

         std::shared_ptr<ITexture> mShadowMapTexture;

         FramebufferObject mFramebuffer;

      public:

         ShadowFramebuffer(std::shared_ptr<ITexture> shadowMapTexture);

         ~ShadowFramebuffer() override;

         void SetTextures() override;
         void SetFramebuffers()  override;
         void SetRenderbuffers() override;
         void CleanUp() override;


         void RenderToTexture(bool bBindFramebuffer, const size_t viewportX, const size_t viewportY, const size_t viewportWidth, const size_t viewportHeight, const GLbitfield clearFlag);
      };
   }
}

