#pragma once

#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlasSpaceRequest.h"

using namespace Graphics::Mesh;
using namespace Graphics::OpenGL;
using namespace Graphics::Texture;

namespace Graphics
{
   namespace Data
   {

      struct CubemapRenderData
      {
         std::shared_ptr<Skin> m_skin;
         std::shared_ptr<ShaderBase> m_shader;
         TextureAtlasSpaceRequest m_texture;

         CubemapRenderData(std::shared_ptr<Skin> cubeMesh,
            std::shared_ptr<ShaderBase> cubemapRendererShader, TextureAtlasSpaceRequest texture)
            : m_skin(cubeMesh)
            , m_shader(cubemapRendererShader)
            , m_texture(texture)
         {
         }

         ~CubemapRenderData()
         {
         }
      };
   }
}
