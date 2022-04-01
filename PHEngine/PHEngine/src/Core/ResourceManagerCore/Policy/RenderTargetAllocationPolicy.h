#pragma once

#include <memory>
#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Texture/Texture2d.h"

using namespace Graphics::Texture;

namespace Resources
{

   class RenderTargetAllocationPolicy
   {
   public:

      template <typename eTextureType = Texture2d, typename TextureParamsModel>
      static inline std::shared_ptr<ITexture> AllocateMemory(const TextureParamsModel& texParams)
      {
         return std::make_shared<eTextureType>(texParams);
      }

      static inline void DeallocateMemory(std::shared_ptr<ITexture> arg)
      {
         arg->CleanUp();
      }
   };

}
