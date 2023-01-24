#pragma once

#include <memory>

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Graphics::OpenGL;
using namespace Graphics::Texture;

namespace Graphics
{
   namespace Data
   {

      struct BillboardRenderData
      {
         std::shared_ptr<Shader> m_shader;
         std::shared_ptr<ITexture> m_texture;

         BillboardRenderData(std::shared_ptr<Shader> billboardShader,
                             std::shared_ptr<ITexture> texture)
             : m_shader(billboardShader),
               m_texture(texture)
         {
         }

         ~BillboardRenderData()
         {
         }
      };
   }
}
