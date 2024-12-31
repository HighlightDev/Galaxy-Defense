#pragma once

#include <memory>
#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Texture/Texture2d.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace Graphics::Texture;
using namespace EngineCore;

namespace Resources
{

   class RenderTargetAllocationPolicy
   {
   public:

      template <typename eTextureType = Texture2d, typename TextureParamsModel>
      static inline std::shared_ptr<ITexture> AllocateMemory(const TextureParamsModel& texParams)
      {
         LogInfo("RenderTargetAllocationPolicy::AllocateMemory: width ", texParams.TexBufferWidth, ", height ", texParams.TexBufferHeight);
         return std::make_shared<eTextureType>(texParams);
      }

      static inline void DeallocateMemory(std::shared_ptr<ITexture> arg)
      {
         LogInfo("RenderTargetAllocationPolicy::DeallocateMemory");
         arg->CleanUp();
      }
   };

}
