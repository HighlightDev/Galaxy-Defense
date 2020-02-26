#pragma once

#include <memory>
#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Texture/Texture2d.h"

using namespace Graphics::Texture;

namespace Resources
{

   template <typename Model>
   class RenderTargetAllocationPolicy
   {
   public:

      template <typename TextureType = Texture2d>
      static inline std::shared_ptr<ITexture> AllocateMemory(Model& texParams)
      {
         return std::make_shared<TextureType>(texParams);
      }

      static inline void DeallocateMemory(std::shared_ptr<ITexture> arg)
      {
         arg->CleanUp();
      }
   };

}
