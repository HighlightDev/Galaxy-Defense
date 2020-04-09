#pragma once

#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"

using namespace Graphics::Mesh;
using namespace Graphics::OpenGL;

namespace Graphics
{
   namespace Data
   {

      struct PhyShapeDebugRenderData
      {
         std::shared_ptr<Skin> m_skin;
         std::shared_ptr<ShaderBase> m_shader;

         PhyShapeDebugRenderData(std::shared_ptr<Skin> skin,
            std::shared_ptr<ShaderBase> shader)
            : m_skin(skin)
            , m_shader(shader)
         {
         }

      };
   }
}

