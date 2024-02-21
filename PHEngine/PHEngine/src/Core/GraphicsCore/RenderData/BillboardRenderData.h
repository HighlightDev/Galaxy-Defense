#pragma once

#include <memory>

#include "Core/GraphicsCore/Material/MaterialProxy.h"

using namespace Graphics;

namespace Graphics
{
   namespace Data
   {
      struct BillboardRenderData
      {
         std::shared_ptr<MaterialProxy> mMaterialProxy;

         BillboardRenderData(const std::shared_ptr<MaterialProxy> &materialProxy)
             : mMaterialProxy(materialProxy)
         {
         }
      };
   }
}
