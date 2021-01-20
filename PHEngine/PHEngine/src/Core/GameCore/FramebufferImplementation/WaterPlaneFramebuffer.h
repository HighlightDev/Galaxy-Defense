#pragma once

#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferBundle.h"

using namespace Graphics;

namespace Game
{
   namespace FramebufferImpl
   {
      class WaterPlaneFramebuffer :
         public FramebufferBundle
      {
      public:
         
         std::shared_ptr<ITexture> mReflectionTexture;
         std::shared_ptr<ITexture> mRefractionTexture;
         std::shared_ptr<ITexture> mDepthTexture;

         FramebufferObject mReflectionFBO;
         FramebufferObject mRefractionFBO;

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


