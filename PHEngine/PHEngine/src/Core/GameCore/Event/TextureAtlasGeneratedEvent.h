#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Graphics::Texture;

namespace Event
{

   class TextureAtlasGeneratedEvent
      : public TEvent<MultipleDataEventPolicy<TextureType>>
   {
   public:
      using Event_t = TEvent<MultipleDataEventPolicy<TextureType>>::Event_t;
   };

}
