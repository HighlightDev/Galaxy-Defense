#pragma once

#include "Core/GraphicsCore/Texture/ITexture.h"

#include <memory>

namespace Graphics
{
   using namespace Texture;

   class IPostFxPass
   {
      public:

      virtual std::shared_ptr<ITexture> ExecutePostFx(const std::shared_ptr<ITexture>& sceneColorTexture) = 0;

      virtual void CleanUp() = 0;
   };
}