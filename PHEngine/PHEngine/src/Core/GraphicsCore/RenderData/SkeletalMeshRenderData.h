#pragma once

#include <memory>
#include <string>

#include "Core/GraphicsCore/Material/MaterialProxy.h"

using namespace Graphics;

namespace Graphics
{
   namespace Data
   {
      struct SkeletalMeshRenderData
      {
         std::string mModelName;
         std::shared_ptr<MaterialProxy> mMaterialProxy;

         SkeletalMeshRenderData(const std::string &modelName,
                                std::shared_ptr<MaterialProxy> materialProxy)
             : mModelName(modelName),
               mMaterialProxy(materialProxy)
         {
         }

         ~SkeletalMeshRenderData() {}
      };
   }
}
