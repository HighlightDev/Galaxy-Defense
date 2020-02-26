#pragma once

#include "Core/GraphicsCore/OpenGL/Framebuffer/Framebuffer.h"

using namespace Graphics;

namespace Game
{
   namespace FramebufferImpl
   {
      class WaterPlaneFramebuffer :
         public Framebuffer
      {
      public:
         
         std::shared_ptr<ITexture> ReflectionTexture;
         std::shared_ptr<ITexture> RefractionTexture;
         std::shared_ptr<ITexture> DepthTexture;

         WaterPlaneFramebuffer();

         virtual ~WaterPlaneFramebuffer();

      protected:

         virtual void SetTextures() override;

         virtual void SetFramebuffers() override;

         virtual void SetRenderbuffers() override;

         virtual void CleanUp() override;
      };
   }
}


