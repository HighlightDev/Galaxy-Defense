#pragma once

#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Graphics::Mesh;
using namespace Graphics::OpenGL;
using namespace Graphics::Texture;

namespace Graphics
{
   namespace Data
   {

      struct BillboardRenderData
      {
         std::shared_ptr<Skin> m_skin;
         std::shared_ptr<ShaderBase> m_shader;
         std::shared_ptr<ITexture> m_texture;

         BillboardRenderData(std::shared_ptr<Skin> billboardMesh,
            std::shared_ptr<ShaderBase> billboardShader, std::shared_ptr<ITexture> texture)
            : m_skin(billboardMesh)
            , m_shader(billboardShader)
            , m_texture(texture)
         {
         }

         ~BillboardRenderData()
         {
         }
      };
   }
}
