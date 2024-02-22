#include "TextureAtlasFactory.h"
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"
#include "TextureAtlasSpaceRequest.h"
#include "Core/GameCore/Event/TextureAtlasGeneratedEvent.h"

#include <algorithm>
#include <gl/glew.h>

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

   void TextureAtlasFactory::AddTextureCubeAtlasReservation(size_t requestId, const glm::ivec2& size)
   {
      CubemapReservations.emplace_back(std::make_pair(requestId, size));
   }

   void TextureAtlasFactory::AddTextureAtlasReservation(size_t requestId, const glm::ivec2& size)
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

   TextureAtlasSpaceRequest TextureAtlasFactory::AddTextureAtlasRequest(const glm::ivec2& size)
   {
      TextureAtlasSpaceRequest obtainer;
      AddTextureAtlasReservation(obtainer.MyRequestId, size);
      return obtainer;
   }

   TextureAtlasSpaceRequest TextureAtlasFactory::AddTextureCubeAtlasRequest(const glm::ivec2& size)
   {
      TextureAtlasSpaceRequest obtainer;
      AddTextureCubeAtlasReservation(obtainer.MyRequestId, size);
      return obtainer;
   }

   void TextureAtlasFactory::AllocateTexture2dAtlasSpace()
   {
      if (!Reservations.size())
         return;

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
            auto texAtlas = std::make_shared<TextureAtlas2D>(atlas);
            m_textureAtlases.push_back(texAtlas);

            auto texAtlas2D = std::static_pointer_cast<TextureAtlas2D>(texAtlas);
            for (auto cellPair : texAtlas2D->Cells)
            {
               mTextureAtlasHandlers[cellPair.first] = std::make_shared<Texture2dAtlasHandler>(texAtlas2D->m_atlasTexture, cellPair.second);
            }

         }
      }

      Reservations.clear();
   }

   void TextureAtlasFactory::AllocateTextureCubeSpace()
   {
      if (!CubemapReservations.size())
         return;

      for (std::vector<std::pair<size_t, glm::ivec2>>::const_iterator it = CubemapReservations.cbegin(); it != CubemapReservations.cend(); ++it)
      { 
         TextureAtlasCube atlas(it->first, std::make_tuple(it->second, it->second, it->second, it->second, it->second, it->second));
         atlas.AllocateReservedMemory();
         auto texAtlas = std::make_shared<TextureAtlasCube>(atlas);
         m_textureAtlases.push_back(texAtlas);

         mTextureAtlasHandlers[texAtlas->m_sizes.first] = std::make_shared<TextureCubeAtlasHandler>(texAtlas->m_atlasTexture);

      }
   }

   void TextureAtlasFactory::AllocateAtlasSpace()
   {
      AllocateTexture2dAtlasSpace();
      AllocateTextureCubeSpace();

      std::for_each(mTextureAtlasHandlers.begin(), mTextureAtlasHandlers.end(), [](const auto& texAtlasHandlerPair) { texAtlasHandlerPair.second->NotifyTextureAtlasBuilded(); });

      Event::TextureAtlasGeneratedGameThreadEvent::GetInstance()->SendEvent(Event::eExecutionOrder::PRE_EXECUTION, eTextureType::TEXTURE_2D);
      Event::TextureAtlasGeneratedGameThreadEvent::GetInstance()->SendEvent(Event::eExecutionOrder::PRE_EXECUTION, eTextureType::TEXTURE_CUBE);

   }

   std::shared_ptr<TextureAtlasHandler> TextureAtlasFactory::GetTextureAtlasCellByRequestId(size_t requestId) const
   {
      assert(mTextureAtlasHandlers.count(requestId));
      return mTextureAtlasHandlers.at(requestId);
   }

   void TextureAtlasFactory::DeallocateTextureAtlasByRequestId(size_t requestId)
   {
      for (auto& atlas : m_textureAtlases)
      {
         if (atlas->GetType() == eTextureType::TEXTURE_2D)
         {
            const auto ptrSp = std::static_pointer_cast<TextureAtlas2D>(atlas);
            std::map<size_t, TextureAtlasCell>::const_iterator it = ptrSp->Cells.find(requestId);

            size_t cellsCount = ptrSp->Cells.size();
            
            if (it != ptrSp->Cells.end())
            {
               // if someone is using this texture atlas, just remove cell
               // if nobody is using, delete texture
               if (cellsCount > 1) 
               {
                  ptrSp->Cells.erase(it);
               }
               else 
               {
                  atlas->DeallocateMemory();
                  m_textureAtlases.erase(std::remove(m_textureAtlases.begin(), m_textureAtlases.end(), atlas), m_textureAtlases.end());
               }
               break;
            }
         }
         else if (atlas->GetType() == eTextureType::TEXTURE_CUBE)
         {
            const auto ptrSp = std::static_pointer_cast<TextureAtlasCube>(atlas);
            if (ptrSp->m_sizes.first == requestId)
            {
               atlas->DeallocateMemory();
               m_textureAtlases.erase(std::remove(m_textureAtlases.begin(), m_textureAtlases.end(), atlas), m_textureAtlases.end());
               break;
            }
         }
      }
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
