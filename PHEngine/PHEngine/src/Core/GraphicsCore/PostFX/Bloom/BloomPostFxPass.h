#pragma once

#include "Core/GraphicsCore/PostFX/IPostFxPass.h"

namespace Graphics
{
   class BloomPostFxPass :
      public IPostFxPass
   {
   public:
      BloomPostFxPass();
      virtual ~BloomPostFxPass();

      virtual std::shared_ptr<ITexture> ExecutePostFx(const std::shared_ptr<ITexture>& sceneColorTexture) override;
   };
}

