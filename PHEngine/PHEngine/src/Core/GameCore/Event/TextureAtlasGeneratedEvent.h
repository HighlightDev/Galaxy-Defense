#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Graphics::Texture;

namespace Event
{

   class TextureAtlasGeneratedEvent
      : public TEvent<MultipleEventPolicy<TextureType>>
   {
   public:
      using Event_t = TEvent<MultipleEventPolicy<TextureType>>::Event_t;
   };

}
