#pragma once
#include "Core/GraphicsCore/OpenGL/Framebuffer/Framebuffer.h"

using namespace Graphics;

namespace Game
{
   namespace FramebufferImpl
   {

      class ShadowFramebuffer :
         public Framebuffer
      {
         using sharedTexture_t = std::shared_ptr<ITexture>;
         using Base = Framebuffer;

         sharedTexture_t m_shadowmapTextureResource;

      public:

         ShadowFramebuffer(sharedTexture_t shadowmapTextureResource);

         virtual ~ShadowFramebuffer();

         virtual void SetTextures() override;
         virtual void SetFramebuffers()  override;
         virtual void SetRenderbuffers() override;
         virtual void CleanUp() override;
      };
   }
}

