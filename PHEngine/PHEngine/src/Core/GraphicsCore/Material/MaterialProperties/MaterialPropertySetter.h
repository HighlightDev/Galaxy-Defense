#pragma once
#include <string>
#include <memory>

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "MaterialProperty.h"

namespace Resources {
   struct IDeferredResourceCreator;
}

namespace EngineCore {
   class GameObject;
}

namespace Graphics
{
   using namespace Texture;

   using Resources::IDeferredResourceCreator;
   using EngineCore::GameObject;

   struct MaterialPropertySetter
   {
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, ITexture* texture);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, std::shared_ptr<ITexture> texture);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, const float value);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, IDeferredResourceCreator* deferredResourceCreator);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const class GameObject* gameObject,const std::string& gamePropertyName, const std::string& bindingName);

   private:
      static class DynamicMaterial* TryCastToDynamicMaterial(class IMaterial* materialIsntance);

      static void SetTextureValue(std::shared_ptr<MaterialProperty> materialProperty, ITexture* texture);
      static void SetTextureValue(std::shared_ptr<MaterialProperty> materialProperty, std::shared_ptr<ITexture> texture);

      static void SetFloatValue(std::shared_ptr<MaterialProperty> materialProperty, const float value);

      static void SetDeferredResourceValue(std::shared_ptr<MaterialProperty> materialProperty, IDeferredResourceCreator* deferredResourceCreator);

      static bool IsPropertyBindingType(std::shared_ptr<MaterialProperty> property, MaterialProperty::eMaterialPropertyType& outPropertyType);
   };
}

