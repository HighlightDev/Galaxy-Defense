#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Graphics::Texture;

namespace Event
{
   class TextureAtlasGeneratedGameThreadEvent
      : public TEvent<TextureAtlasGeneratedGameThreadEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<eTextureType>>
   {
   public:
      using Event_t = TEvent<TextureAtlasGeneratedGameThreadEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<eTextureType>>::Event_t;

      std::string ToString() const override {
         return "TextureAtlasGeneratedGameThreadEvent";
      }
   };

}
