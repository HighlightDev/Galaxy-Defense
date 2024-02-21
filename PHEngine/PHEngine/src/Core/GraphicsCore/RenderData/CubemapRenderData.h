#pragma once

#include <memory>

#include "Core/GraphicsCore/TextureAtlas/TextureAtlasSpaceRequest.h"

namespace Graphics
{
   namespace Data
   {
      struct CubemapRenderData
      {
         TextureAtlasSpaceRequest m_texture;

         CubemapRenderData(TextureAtlasSpaceRequest texture)
             : m_texture(texture)
         {
         }
      };
   }
}
