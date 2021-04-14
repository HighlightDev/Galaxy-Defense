#pragma once
#include <string>
#include <memory>

#include "Core/GraphicsCore/Texture/ITexture.h"

namespace Resources {
   struct IDeferredResourceCreator;
}

namespace Graphics
{
   using namespace Texture;

   using Resources::IDeferredResourceCreator;

   struct MaterialProperty;

   struct MaterialPropertySetter
   {
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, ITexture* texture);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, std::shared_ptr<ITexture> texture);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, const float value);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, IDeferredResourceCreator* deferredResourceCreator);

   private:
      static class DynamicMaterial* TryCastToDynamicMaterial(class IMaterial* materialIsntance);

      static void SetTextureValue(std::shared_ptr<MaterialProperty> materialProperty, ITexture* texture);
      static void SetTextureValue(std::shared_ptr<MaterialProperty> materialProperty, std::shared_ptr<ITexture> texture);

      static void SetFloatValue(std::shared_ptr<MaterialProperty> materialProperty, const float value);

      static void SetDeferredResourceValue(std::shared_ptr<MaterialProperty> materialProperty, IDeferredResourceCreator* deferredResourceCreator);
   };
}

