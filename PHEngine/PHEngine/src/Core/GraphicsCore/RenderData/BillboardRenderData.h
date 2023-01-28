#pragma once

#include <memory>

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/Material/MaterialProxy.h"

using namespace Graphics::OpenGL;
using namespace Graphics;

namespace Graphics
{
   namespace Data
   {
      struct BillboardRenderData
      {
         std::shared_ptr<IShader> m_shader;
         std::shared_ptr<MaterialProxy> mMaterialProxy;

         BillboardRenderData(const std::shared_ptr<IShader> &billboardShader,
                             const std::shared_ptr<MaterialProxy> &materialProxy)
             : m_shader(billboardShader),
               mMaterialProxy(materialProxy)
         {
         }

         ~BillboardRenderData()
         {
         }
      };
   }
}
