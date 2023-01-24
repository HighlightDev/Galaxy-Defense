#pragma once

#include <memory>

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlasSpaceRequest.h"

using namespace Graphics::OpenGL;
using namespace Graphics::Texture;

namespace Graphics
{
   namespace Data
   {
      struct CubemapRenderData
      {
         std::shared_ptr<Shader> m_shader;
         TextureAtlasSpaceRequest m_texture;

         CubemapRenderData(std::shared_ptr<Shader> cubemapRendererShader,
                           TextureAtlasSpaceRequest texture)
             : m_shader(cubemapRendererShader),
               m_texture(texture)
         {
         }

         ~CubemapRenderData()
         {
         }
      };
   }
}
