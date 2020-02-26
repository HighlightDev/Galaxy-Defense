#pragma once

#include "Core/GraphicsCore/Texture/ITexture.h"

#include <memory>

using namespace Graphics::Texture;

struct TexturePropertyProcessPolicy
{
   using property_t = std::shared_ptr<ITexture>;
   using propertyValue_t = int32_t;

   static propertyValue_t Process(property_t property, size_t propertyIndex)
   {
      property->BindTexture(10 + propertyIndex); // 10 is in case if previous texture slots are not available
      return propertyIndex + 10;
   }
};
