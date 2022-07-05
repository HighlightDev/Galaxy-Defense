#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Graphics::Texture;

namespace Event
{

   class TextureAtlasGeneratedEvent
      : public TEvent<MultipleDataEventPolicy<eTextureType>>
   {
   public:
      using Event_t = TEvent<MultipleDataEventPolicy<eTextureType>>::Event_t;

      virtual std::string ToString() const override {
         return "TextureAtlasGeneratedEvent";
      }
   };

}
