#pragma once

#include <memory>

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GameCore/ShaderImplementation/WaterPlaneShader.h"

using namespace Graphics::Texture;
using namespace EngineCore::ShaderImpl;

namespace Graphics
{

   namespace Data
   {
      struct WaterPlaneRenderData
      {
         std::shared_ptr<IShader> m_materialShader;
         std::shared_ptr<MaterialProxy> mMaterialProxy;

         WaterPlaneRenderData(std::shared_ptr<IShader> materialShader,
                              std::shared_ptr<MaterialProxy> materialProxy)
             : m_materialShader(materialShader),
               mMaterialProxy(materialProxy)
         {
         }
      };
   }

}
