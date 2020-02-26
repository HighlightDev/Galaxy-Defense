#include "TextureAtlas.h"
#include "TextureAtlasFactory.h"
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"
#include "Core/GraphicsCore/Texture/Texture2d.h"
#include "Core/GraphicsCore/Texture/CubemapTexture.h"

#include <algorithm>

using namespace Resources;
using namespace Graphics::Texture;

namespace Graphics
{
   // ***************** TextureAtlas *****************//
   TextureType TextureAtlas::GetType() const
   {
      return m_type;
   }

   void TextureAtlas::DeallocateMemory()
   {
      if (!m_atlasTexture)
      {
         RenderTargetPool::GetInstance()->TryToFreeMemory(m_atlasTexture);
         m_atlasTexture = std::shared_ptr<ITexture>(nullptr);
      }
   }

   // ***************** TextureAtlas2D *****************//
   void TextureAtlas2D::AllocateReservedMemory()
   {
      if (!m_atlasTexture)
      {
         TexParams shadowMapParams(shadow_map_size, shadow_map_size, GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, 0, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, GL_REPEAT);
         m_atlasTexture = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(shadowMapParams);
      }
   }

   TextureAtlas2D::TextureAtlas2D()
   {
      m_type = TextureType::TEXTURE_2D;
   }

   TextureAtlas2D::~TextureAtlas2D()
   {
   }
  
   void TextureAtlas2D::ShrinkReservedMemory()
   {
      glm::ivec2 max(0);

      for (auto cit = Cells.cbegin(); cit != Cells.cend(); ++cit)
      {
         max.x = std::max(cit->second.X + cit->second.Width, max.x);
         max.y = std::max(cit->second.Y + cit->second.Height, max.y);
      }

      int32_t shadowMapSize = TextureAtlasFactory::SHADOW_MAP_SIZE;
      while (true)
      {
         if (shadowMapSize > max.x && shadowMapSize > max.y)
         {
            int32_t shrinkedShadowMapSize = shadowMapSize >> 1;
            if (shrinkedShadowMapSize >= max.x && shrinkedShadowMapSize >= max.y)
               shadowMapSize = shrinkedShadowMapSize;
            else
               break;
         }
         else
            break;
      }

      shadow_map_size = shadowMapSize;

      for (auto it = Cells.begin(); it != Cells.end(); ++it)
      {
         it->second.TotalShadowMapWidth = shadow_map_size;
         it->second.TotalShadowMapHeight = shadow_map_size;
      }
   }

   // ***************** TextureAtlasCube *****************//

   TextureAtlasCube::TextureAtlasCube(size_t requestId, std::tuple<glm::ivec2, glm::ivec2, glm::ivec2, glm::ivec2, glm::ivec2, glm::ivec2> sizes)
   {
      m_type = TextureType::TEXTURE_CUBE;
      m_sizes = std::make_pair(requestId, sizes);
      auto size = std::get<0>(m_sizes.second);
      shadow_map_size = size.x;
   }

   TextureAtlasCube::~TextureAtlasCube()
   {
   }

   void TextureAtlasCube::AllocateReservedMemory()
   {
      if (!m_atlasTexture)
      {
         TexParams shadowMapParams(shadow_map_size, shadow_map_size, GL_TEXTURE_CUBE_MAP, GL_NEAREST, GL_NEAREST, 0, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_REPEAT, true);
         m_atlasTexture = RenderTargetPool::GetInstance()->GetOrAllocateResource<CubemapTexture>(shadowMapParams);
      }
   }

}