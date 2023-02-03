#pragma once

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferBundle.h"

#include <memory>

namespace Graphics
{
   using namespace Texture;

   class IPostFxPass
   {
      public:

      virtual void ExecutePostFx(const std::shared_ptr<ITexture>& sceneColorTexture, const std::shared_ptr<FramebufferBundle>& previousStepFramebuffer) = 0;

      virtual std::shared_ptr<ITexture> GetPostFxResult() const = 0;

      virtual void CleanUp() = 0;
   };
}