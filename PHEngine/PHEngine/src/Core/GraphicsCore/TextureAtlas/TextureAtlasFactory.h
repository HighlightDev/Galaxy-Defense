#pragma once
#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlasHandler.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlas.h"

#include <memory>
#include <map>
#include <stdint.h>
#include <glm/vec2.hpp>
#include <vector>

using namespace Graphics::Texture;

namespace Graphics
{
   class LazyTextureAtlasObtainer;

   class TextureAtlasFactory
   {
      friend class LazyTextureAtlasObtainer;

      static std::unique_ptr<TextureAtlasFactory> m_instance;

      std::vector<std::shared_ptr<TextureAtlas>> m_textureAtlases;

      std::vector<std::pair<size_t, glm::ivec2>> Reservations;
      
      std::vector<std::pair<size_t, glm::ivec2>> CubemapReservations;

   public:

      enum { SHADOW_MAP_SIZE = 1 << 10 };

      TextureAtlasFactory();

      ~TextureAtlasFactory();

      static std::unique_ptr<TextureAtlasFactory>& GetInstance()
      {
         if (!m_instance)
            m_instance = std::make_unique<TextureAtlasFactory>();

         return m_instance;
      }

      void AllocateAtlasSpace();

      LazyTextureAtlasObtainer AddTextureAtlasRequest(glm::ivec2 size);

      LazyTextureAtlasObtainer AddTextureCubeAtlasRequest(glm::ivec2 size);

   private:

      void AllocateTexture2dAtlasSpace();

      void AllocateTextureCubeSpace();

      std::shared_ptr<TextureAtlasHandler> GetTextureAtlasCellByRequestId(size_t requestId) const;

      void AddTextureAtlasReservation(size_t requestId, glm::ivec2 size);

      void AddTextureCubeAtlasReservation(size_t requestId, glm::ivec2 size);

      void SplitChunk(std::vector<TextureAtlasCell>& emptyChunks, std::vector<TextureAtlasCell>::const_iterator splittingEmptyChunkIt, TextureAtlasCell& splitCenterCell);
   };

  
}

