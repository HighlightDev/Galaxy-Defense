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
         std::shared_ptr<ShaderBase> m_shader;
         std::shared_ptr<ITexture> m_dayTex;
         std::shared_ptr<ITexture> m_nightTex;

         SkyboxRenderData(std::shared_ptr<Skin> skyboxMesh,
            std::shared_ptr<ShaderBase> skyboxShader, std::shared_ptr<ITexture> dayTexture, std::shared_ptr<ITexture> nightTexture)
            : m_skin(skyboxMesh)
            , m_shader(skyboxShader)
            , m_dayTex(dayTexture)
            , m_nightTex(nightTexture)
         {
         }
         ~SkyboxRenderData()
         {
         }
      };
   }
}
