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

      struct SkyboxRenderData
      {

         std::shared_ptr<Skin> m_skin;
         std::shared_ptr<ICompositeShader> m_materialShader;

         SkyboxRenderData(std::shared_ptr<Skin> skyboxMesh,
            std::shared_ptr<IShader> materialShader)
            : m_skin(skyboxMesh)
            , m_materialShader(std::dynamic_pointer_cast<ICompositeShader>(materialShader))
         {
         }

         ~SkyboxRenderData()
         {
         }
      };
   }
}
