#pragma once

#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferBundle.h"

using namespace Graphics;

namespace Game {

   namespace FramebufferImpl
   {
      class PlanarReflectionFramebuffer :
         public FramebufferBundle
      {
         ViewPortInfo mReflectionViewPortInfo;

         std::shared_ptr<ITexture> mReflectionTexture;

         FramebufferObject mReflectionFBO;

      public:

         PlanarReflectionFramebuffer(const ViewPortInfo& reflectionViewPortInfo);

         virtual ~PlanarReflectionFramebuffer();

         void RenderToTexture();

      protected:

         virtual void SetTextures() override;

         virtual void SetFramebuffers() override;

         virtual void SetRenderbuffers() override;

         virtual void CleanUp() override;

      };
   }
}

