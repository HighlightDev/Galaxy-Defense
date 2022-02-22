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

         virtual ~ShadowFramebuffer();

         virtual void SetTextures() override;
         virtual void SetFramebuffers()  override;
         virtual void SetRenderbuffers() override;
         virtual void CleanUp() override;


         void RenderToTexture(bool bBindFramebuffer, const size_t viewportX, const size_t viewportY, const size_t viewportWidth, const size_t viewportHeight, const GLbitfield clearFlag);
      };
   }
}

