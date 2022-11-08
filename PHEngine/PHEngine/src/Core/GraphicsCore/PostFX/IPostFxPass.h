#pragma once

#include "Core/GraphicsCore/Texture/ITexture.h"

#include <memory>

namespace Graphics
{
   using namespace Texture;

   class IPostFxPass
   {
      public:

      virtual void ExecutePostFx(const std::shared_ptr<ITexture>& sceneColorTexture) = 0;

      virtual std::shared_ptr<ITexture> GetPostFxResult() const = 0;

      virtual void CleanUp() = 0;
   };
}