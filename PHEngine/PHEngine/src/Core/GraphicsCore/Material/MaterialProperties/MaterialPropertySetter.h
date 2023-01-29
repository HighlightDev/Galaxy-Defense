#pragma once
#include <string>
#include <memory>
#include <glm/vec2.hpp>

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "MaterialProperty.h"

namespace Resources {
   struct IDeferredResourceCreator;
}

namespace EngineCore {
   class EngineObject;
}

namespace Graphics
{
   using namespace Texture;

   using Resources::IDeferredResourceCreator;
   using EngineCore::EngineObject;

   struct MaterialPropertySetter
   {
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, ITexture* texture);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, std::shared_ptr<ITexture> texture);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, const float value);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, IDeferredResourceCreator* deferredResourceCreator);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const class EngineObject* gameObject,const std::string& gamePropertyName, const std::string& bindingName);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, const glm::ivec2& value);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, const glm::vec2& value);

   private:
      static class DynamicMaterial* TryCastToDynamicMaterial(class IMaterial* materialIsntance);

      static void SetTextureValue(std::shared_ptr<MaterialProperty> materialProperty, ITexture* texture);
      static void SetTextureValue(std::shared_ptr<MaterialProperty> materialProperty, std::shared_ptr<ITexture> texture);

      static void SetFloatValue(std::shared_ptr<MaterialProperty> materialProperty, const float value);

      static void SetIVec2Value(std::shared_ptr<MaterialProperty> materialProperty, const glm::ivec2 &value);

      static void SetVec2Value(std::shared_ptr<MaterialProperty> materialProperty, const glm::vec2& value);

      static void SetDeferredResourceValue(std::shared_ptr<MaterialProperty> materialProperty, IDeferredResourceCreator* deferredResourceCreator);

      static bool IsPropertyBindingType(std::shared_ptr<MaterialProperty> property);
   };
}

