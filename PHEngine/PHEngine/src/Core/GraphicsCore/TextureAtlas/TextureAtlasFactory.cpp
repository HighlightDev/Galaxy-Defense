#include "TextureAtlasFactory.h"
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"
#include "LazyTextureAtlasObtainer.h"

#include <algorithm>
#include <GL/glew.h>

using namespace Resources;

namespace Graphics
{

   std::unique_ptr<TextureAtlasFactory> TextureAtlasFactory::m_instance;

   TextureAtlasFactory::TextureAtlasFactory()
   {
   }

   TextureAtlasFactory::~TextureAtlasFactory()
   {
   }

   void TextureAtlasFactory::AddTextureCubeAtlasReservation(size_t requestId, glm::ivec2 size)
   {
      CubemapReservations.emplace_back(std::make_pair(requestId, size));
   }

   void TextureAtlasFactory::AddTextureAtlasReservation(size_t requestId, glm::ivec2 size)
   {
      if (Reservations.size() == 0)
      {
         Reservations.emplace_back(std::make_pair(requestId, size));
      }
      else
      {
         const auto calcSquare = [](const glm::ivec2& quad) -> int32_t {  return (quad.x * quad.y); };
         bool bInserted = false;

         // Inserting new reservation size according to ascending rule
         int32_t index = 0;
         for (auto cit = Reservations.cbegin(); cit != Reservations.end(); ++cit, index++)
         {
            if (calcSquare((cit->second)) <= calcSquare(size))
            {
               Reservations.insert(cit, std::make_pair(requestId, size));
               bInserted = true;
               break;
            }
         }

         // If new reservation size is the lowest - just insert it in the end
         if (!bInserted)
         {
            Reservations.emplace_back(std::make_pair(requestId, size));
         }
      }
   }

   LazyTextureAtlasObtainer TextureAtlasFactory::AddTextureAtlasRequest(glm::ivec2 size)
   {
      LazyTextureAtlasObtainer obtainer;
      AddTextureAtlasReservation(obtainer.MyRequestId, size);
      return obtainer;
   }

   LazyTextureAtlasObtainer TextureAtlasFactory::AddTextureCubeAtlasRequest(glm::ivec2 size)
   {
      LazyTextureAtlasObtainer obtainer;
      AddTextureCubeAtlasReservation(obtainer.MyRequestId, size);
      return obtainer;
   }

   void TextureAtlasFactory::AllocateTexture2dAtlasSpace()
   {
      auto getRelevantEmptyChunk = [](const std::vector<TextureAtlasCell>& emptyChunks, const glm::ivec2& reservation)
      {
         size_t reverseIndex = emptyChunks.size() - 1;
         std::vector<TextureAtlasCell>::const_iterator result = emptyChunks.end();
         for (auto rit = emptyChunks.rbegin(); rit != emptyChunks.rend(); ++rit, --reverseIndex)
         {
            if (rit->Width >= reservation.x && rit->Height >= reservation.y)
            {
               // enough space for cell in empty chunk
               result = emptyChunks.begin() + reverseIndex;
               break;
            }
         }

         return result;
      };

      while (Reservations.size())
      {
         TextureAtlas2D atlas;
         std::map<size_t, TextureAtlasCell>& cells = atlas.Cells;
         std::vector<TextureAtlasCell> emptyChunks = { TextureAtlasCell(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE) };

         auto it = Reservations.begin();
         while (it != Reservations.end())
         {
            auto relevantChunkIt = getRelevantEmptyChunk(emptyChunks, it->second);
            if (relevantChunkIt != emptyChunks.end())
            {
               TextureAtlasCell newCell(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, relevantChunkIt->X, relevantChunkIt->Y, it->second.x, it->second.y);
               cells.insert(std::make_pair(it->first, newCell));
               SplitChunk(emptyChunks, relevantChunkIt, newCell);

               it = Reservations.erase(it);
            }
            else
            {
               ++it;
            }
         }
         if (atlas.Cells.size())
         {
            atlas.ShrinkReservedMemory();
            atlas.AllocateReservedMemory();
            m_textureAtlases.push_back(std::make_shared<TextureAtlas2D>(atlas));
         }
      }

      Reservations.clear();
   }

   void TextureAtlasFactory::AllocateTextureCubeSpace()
   {
      for (std::vector<std::pair<size_t, glm::ivec2>>::const_iterator it = CubemapReservations.cbegin(); it != CubemapReservations.cend(); ++it)
      { 
         TextureAtlasCube atlas(it->first, std::make_tuple(it->second, it->second, it->second, it->second, it->second, it->second));
         atlas.AllocateReservedMemory();
         m_textureAtlases.push_back(std::make_shared<TextureAtlasCube>(atlas));
      }
   }

   void TextureAtlasFactory::AllocateAtlasSpace()
   {
      AllocateTexture2dAtlasSpace();
      AllocateTextureCubeSpace();
   }

   std::shared_ptr<TextureAtlasHandler> TextureAtlasFactory::GetTextureAtlasCellByRequestId(size_t requestId) const
   {
      std::shared_ptr<TextureAtlasHandler> result;

      for (auto& atlas : m_textureAtlases)
      {
         if (atlas->GetType() == TextureType::TEXTURE_2D)
         {
            TextureAtlas2D* ptr = static_cast<TextureAtlas2D*>(atlas.get());
            std::map<size_t, TextureAtlasCell>::const_iterator it = ptr->Cells.find(requestId);
            if (it != ptr->Cells.end())
            {
               result = std::make_shared<Texture2dAtlasHandler>(atlas->m_atlasTexture, it->second);
               break;
            }
         }
         else if (atlas->GetType() == TextureType::TEXTURE_CUBE)
         {
            TextureAtlasCube* ptr = static_cast<TextureAtlasCube*>(atlas.get());
            if (ptr->m_sizes.first == requestId)
            {
               result = std::make_shared<TextureCubeAtlasHandler>(std::move(atlas->m_atlasTexture));
               break;
            }
         }
      }

      return result;
   }

   void TextureAtlasFactory::SplitChunk(std::vector<TextureAtlasCell>& emptyChunks, std::vector<TextureAtlasCell>::const_iterator splittingEmptyChunkIt, TextureAtlasCell& splitCenterCell)
   {
      TextureAtlasCell leftTopCell = TextureAtlasCell(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, splittingEmptyChunkIt->X, splittingEmptyChunkIt->Y + splitCenterCell.Height, splitCenterCell.Width, splittingEmptyChunkIt->Height - splitCenterCell.Height);
      TextureAtlasCell rightBottomCell = TextureAtlasCell(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, splittingEmptyChunkIt->X + splitCenterCell.Width, splittingEmptyChunkIt->Y, splittingEmptyChunkIt->Width - splitCenterCell.Width, splitCenterCell.Height);
      TextureAtlasCell rightTopCell = TextureAtlasCell(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, splittingEmptyChunkIt->X + splitCenterCell.Width, splittingEmptyChunkIt->Y + splitCenterCell.Height, splittingEmptyChunkIt->Width - splitCenterCell.Width, splittingEmptyChunkIt->Height - splitCenterCell.Height);

      // Remove splitting empty chunk because it was split
      emptyChunks.erase(splittingEmptyChunkIt);

      auto sortFunctor = [](const TextureAtlasCell& atlasCell1, const TextureAtlasCell& atlasCell2) -> bool
      {
         int32_t atlas1Square = atlasCell1.Height * atlasCell1.Width;
         int32_t atlas2Square = atlasCell2.Height * atlasCell2.Width;
         if (atlas1Square > atlas2Square)
         {
            return true;
         }
         else if (atlas1Square == atlas2Square)
         {
            return (atlasCell1.Height > atlasCell2.Height);
         }
         else return false;
      };

      emptyChunks.emplace_back(leftTopCell);
      emptyChunks.emplace_back(rightTopCell);
      emptyChunks.emplace_back(rightBottomCell);

      std::sort(emptyChunks.begin(), emptyChunks.end(), sortFunctor);
   }

}
