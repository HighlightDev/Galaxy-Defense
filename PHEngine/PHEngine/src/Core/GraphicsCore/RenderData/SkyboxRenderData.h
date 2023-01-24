#pragma once

#include <memory>

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Material/MaterialProxy.h"

using namespace Graphics::OpenGL;
using namespace Graphics::Texture;

namespace Graphics
{
   namespace Data
   {

      struct SkyboxRenderData
      {
         std::shared_ptr<IShader> m_materialShader;
         std::shared_ptr<IShader> m_planarReflectionShader;
         std::shared_ptr<MaterialProxy> mMaterialProxy;

         SkyboxRenderData(std::shared_ptr<IShader> materialShader,
                          std::shared_ptr<IShader> planarReflectionShader,
                          std::shared_ptr<MaterialProxy> materialProxy)
             : m_materialShader(materialShader),
               m_planarReflectionShader(planarReflectionShader),
               mMaterialProxy(materialProxy)
         {
         }

         ~SkyboxRenderData()
         {
         }
      };
   }
}
