#pragma once

#include "Core/GraphicsCore/TextureAtlas/TextureAtlasCell.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlas.h"

#include <memory>

using namespace Graphics::Texture;

namespace Graphics
{
   struct TextureAtlasHandler
   {
   protected:

      std::shared_ptr<ITexture> m_atlasResource;

      TextureType m_texType;

   public:

      TextureAtlasHandler(const std::shared_ptr<ITexture>& resource)
         : m_atlasResource(resource)
         , m_texType(TextureType::UNDEFINED)
      {
      }

      inline std::shared_ptr<ITexture> GetAtlasResource() const
      {
         return m_atlasResource;
      }

      inline TextureType GetTextureType() const
      {
         return m_texType;
      }
   };

   struct Texture2dAtlasHandler : public TextureAtlasHandler
   {
   protected:

      TextureAtlasCell m_atlasCell;

   public:

      Texture2dAtlasHandler(const std::shared_ptr<ITexture>& resource, const TextureAtlasCell& cell)
         : TextureAtlasHandler(resource)
         , m_atlasCell(cell)
      {
         m_texType = TextureType::TEXTURE_2D;
      }

      inline TextureAtlasCell GetAtlasCell() const
      {
         return m_atlasCell;
      }
   };

   struct TextureCubeAtlasHandler : public TextureAtlasHandler
   {
      TextureCubeAtlasHandler(const std::shared_ptr<ITexture>& resource)
         : TextureAtlasHandler(resource)
      {
         m_texType = TextureType::TEXTURE_CUBE;
      }
   };
}
