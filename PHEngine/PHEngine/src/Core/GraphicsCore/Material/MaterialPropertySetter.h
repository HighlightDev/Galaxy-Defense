#pragma once
#include <string>
#include <memory>

#include "Core/GraphicsCore/Texture/ITexture.h"

namespace Graphics
{
   using namespace Texture;

   struct MaterialPropertySetter
   {
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, ITexture* texture);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, std::shared_ptr<ITexture> texture);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, float value);
   };
}

