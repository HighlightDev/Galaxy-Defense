#pragma once

#include <memory>

#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Graphics::Texture;

namespace Graphics
{

   namespace Data
   {
      struct ForwardShadingMeshRenderData
      {
         std::string mModelPath;
         std::shared_ptr<IShader> m_materialShader;
         std::shared_ptr<MaterialProxy> mMaterialProxy;

         ForwardShadingMeshRenderData(const std::string &modelPath,
                                      const std::shared_ptr<IShader> &materialShader,
                                      const std::shared_ptr<MaterialProxy> &materialProxy)
             : mModelPath(modelPath),
               m_materialShader(materialShader),
               mMaterialProxy(materialProxy)
         {
         }
      };
   }

}
