#pragma once

#include "TextureAtlasCell.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

#include <memory>
#include <map>
#include <tuple>
#include <glm/vec2.hpp>

using namespace Graphics::Texture;

namespace Graphics
{

   class TextureAtlas
   {
      friend class TextureAtlasFactory;

      virtual void AllocateReservedMemory() = 0;

      void DeallocateMemory();

   protected:

      int32_t shadow_map_size;

      std::shared_ptr<ITexture> m_atlasTexture;

      eTextureType m_type = eTextureType::UNDEFINED;

   public:

      ~TextureAtlas() = default;

      eTextureType GetType() const;
   };

   class TextureAtlas2D : public TextureAtlas
   {
      friend class TextureAtlasFactory;

      std::map<size_t, TextureAtlasCell> Cells;

      void AllocateReservedMemory() override;

      void ShrinkReservedMemory();

   public: 

      explicit TextureAtlas2D();

      ~TextureAtlas2D();

      TextureAtlas2D(const TextureAtlas2D&) = default;
   };

   class TextureAtlasCube : public TextureAtlas
   {
      friend class TextureAtlasFactory;

      std::pair<size_t, std::tuple<glm::ivec2, glm::ivec2, glm::ivec2, glm::ivec2, glm::ivec2, glm::ivec2>> m_sizes;

      void AllocateReservedMemory() override;

   public:

      explicit TextureAtlasCube(size_t requestId, std::tuple<glm::ivec2, glm::ivec2, glm::ivec2, glm::ivec2, glm::ivec2, glm::ivec2> m_sizes);

      ~TextureAtlasCube();

      TextureAtlasCube(const TextureAtlasCube&) = default;
   };
}