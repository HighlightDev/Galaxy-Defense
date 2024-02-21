#pragma once

#include <memory>

#include "Core/GraphicsCore/Material/MaterialProxy.h"

namespace Graphics
{
   namespace Data
   {
      struct SkyboxRenderData
      {
         std::shared_ptr<MaterialProxy> mMaterialProxy;

         SkyboxRenderData(std::shared_ptr<MaterialProxy> materialProxy)
             : mMaterialProxy(materialProxy)
         {
         }
      };
   }
}
