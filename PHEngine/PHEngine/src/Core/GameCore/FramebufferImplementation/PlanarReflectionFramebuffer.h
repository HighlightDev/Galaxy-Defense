#pragma once

#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferBundle.h"

using namespace Graphics;

namespace EngineCore {

   namespace FramebufferImpl
   {
      class PlanarReflectionFramebuffer :
         public FramebufferBundle
      {
         ViewPortInfo mReflectionViewPortInfo;

         std::shared_ptr<ITexture> mReflectionSurfaceTarget;
         std::shared_ptr<ITexture> mReflectionTexture;

         FramebufferObject mReflectionRenderTargetSurface;
         FramebufferObject mReflectionTextureSurface;

      public:

         PlanarReflectionFramebuffer(const ViewPortInfo& reflectionViewPortInfo);

         virtual ~PlanarReflectionFramebuffer();

         void RenderToTexture();

         void ResolveReflectionRenderTargetSurfaceData();

         std::shared_ptr<ITexture> GetReflectionTexture() const;

      protected:

         virtual void SetTextures() override;

         virtual void SetFramebuffers() override;

         virtual void SetRenderbuffers() override;

         virtual void CleanUp() override;

      };
   }
}

