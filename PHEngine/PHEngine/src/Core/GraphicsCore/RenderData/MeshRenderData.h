#pragma once

#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Graphics::Mesh;
using namespace Graphics::Texture;

namespace Graphics
{
   namespace Data
   {
      struct MeshRenderData
      {
         std::string mModelPath;
         std::shared_ptr<MaterialProxy> mMaterialProxy;
         bool mIsDeferredShaded;

         MeshRenderData(
             const std::string& modelPath,
             std::shared_ptr<MaterialProxy> materialProxy,
             const bool isDeferredShaded)
             : mModelPath(modelPath),
               mMaterialProxy(materialProxy),
               mIsDeferredShaded(isDeferredShaded)
         {
         }
      };
   }
}
