#pragma once
#include <memory>

#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferBundle.h"

using namespace Graphics;

namespace Resources
{

   class RenderTargetStorage
   {
   public:
      RenderTargetStorage();

      ~RenderTargetStorage();

      void AddRenderTarget(std::shared_ptr<FramebufferBundle> frambuffer);
   };
}

