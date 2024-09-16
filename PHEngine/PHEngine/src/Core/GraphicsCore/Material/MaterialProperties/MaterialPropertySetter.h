#pragma once
#include <string>
#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "MaterialProperty.h"

namespace Resources
{
   class IDeferredResourceCreator;
   class MaterialInstanceDataProvider;
}

namespace EngineCore
{
   class EngineObject;
}

namespace Graphics
{
   class IMaterial;
   class DynamicMaterial;

   struct MaterialPropertySetter
   {
      static void SetMaterialPropertyValue(const std::shared_ptr<IMaterial> &materialInstance, const std::string &propertyName, const std::shared_ptr<::Graphics::Texture::ITexture> &texture);
      static void SetMaterialPropertyValue(const std::shared_ptr<IMaterial> &materialInstance, const std::string &propertyName, const float value);
      static void SetMaterialPropertyValue(const std::shared_ptr<IMaterial> &materialInstance, const std::string &propertyName, const std::shared_ptr<::Resources::IDeferredResourceCreator> &deferredResourceCreator);
      static void SetMaterialPropertyValue(const std::shared_ptr<IMaterial> &materialInstance,
                                           const std::shared_ptr<::EngineCore::EngineObject> &gameObjectSp,
                                           const std::string &gamePropertyName,
                                           const std::string &bindingName);

      static void SetMaterialInstancedPropertyValue(const std::shared_ptr<IMaterial> &materialInstance,
                                                    const std::shared_ptr<::Resources::MaterialInstanceDataProvider> &instanceDataProvider,
                                                    const std::shared_ptr<::EngineCore::EngineObject> &gameObjectSp,
                                                    const std::string &gamePropertyName,
                                                    const std::string &bindingName);

      static void SetMaterialPropertyValue(const std::shared_ptr<IMaterial> &materialInstance, const std::string &propertyName, const glm::ivec2 &value);
      static void SetMaterialPropertyValue(const std::shared_ptr<IMaterial> &materialInstance, const std::string &propertyName, const glm::vec2 &value);
      static void SetMaterialPropertyValue(const std::shared_ptr<IMaterial> &materialInstance, const std::string &propertyName, const glm::vec3 &value);

   private:
      static std::shared_ptr<DynamicMaterial> TryCastToDynamicMaterial(const std::shared_ptr<IMaterial> &materialIsntance);
      static bool IsPropertyBindingType(const std::shared_ptr<MaterialProperty> &property);

      static void SetDeferredResourceValue(const std::shared_ptr<MaterialProperty> &materialProperty, const std::shared_ptr<::Resources::IDeferredResourceCreator> &deferredResourceCreator);
      static void SetTextureValue(const std::shared_ptr<MaterialProperty> &materialProperty, const std::shared_ptr<::Graphics::Texture::ITexture> &texture);
      static void SetFloatValue(const std::shared_ptr<MaterialProperty> &materialProperty, const float value);
      static void SetIVec2Value(const std::shared_ptr<MaterialProperty> &materialProperty, const glm::ivec2 &value);
      static void SetVec2Value(const std::shared_ptr<MaterialProperty> &materialProperty, const glm::vec2 &value);
      static void SetVec3Value(const std::shared_ptr<MaterialProperty> &materialProperty, const glm::vec3 &value);
   };
}
